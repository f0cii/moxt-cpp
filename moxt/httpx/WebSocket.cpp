#include "WebSocket.hpp"
#include <unordered_map>
#include <utility>

void on_message_null(WebSocket *ws, const char *data, size_t len) {}

WebSocket::WebSocket(std::string host, std::string port, std::string path,
                       asio::io_context &ioContext, int tls_version)
    : host(std::move(host)), port(std::move(port)), path(std::move(path)),
      _ioContext(ioContext), _resolver(asio::make_strand(_ioContext)),
      _sslContext(asio::ssl::context::tlsv12_client),
      _reconnectTimer(_ioContext), m_HeartbeatTimer(_ioContext),
      m_HeartbeatInterval(), on_connect_callback_(nullptr),
      on_heartbeat_callback_(nullptr), on_message_callback_(on_message_null) {
    logd("WebSocket::WebSocket");
    //   SSL_CTX *handle = ::SSL_CTX_new(::TLS_method());
    //   // SSL_CTX_set_min_proto_version(handle, TLS1_2_VERSION);
    SSL_CTX_set_min_proto_version(_sslContext.native_handle(), TLS1_1_VERSION);
    SSL_CTX_set_max_proto_version(
        _sslContext.native_handle(),
        tls_version); // tls_version); // TLS1_3_VERSION
    //   auto ctx = std::make_shared<boost::asio::ssl::context>(handle);
    //_sslContext.set_default_verify_paths();
    _sslContext.set_options(boost::asio::ssl::context::default_workarounds |
                            boost::asio::ssl::context::no_sslv2 |
                            boost::asio::ssl::context::no_sslv3 |
                            boost::asio::ssl::context::single_dh_use);
    logd("WebSocket::WebSocket success");
}

WebSocket::~WebSocket() {
    logd("WebSocket::~WebSocket");
    Disconnect();
}

void WebSocket::Connect() {
    logd("WebSocket::Connect");

    _resolver.async_resolve(
        host, port, beast::bind_front_handler(&WebSocket::OnResolve, this));
}

void WebSocket::OnResolve(beast::error_code ec,
                           asio::ip::tcp::resolver::results_type results) {
    logd("WebSocket::OnResolve");

    if (ec) {
        loge("Can't resolve Discord gateway URL '{}': {} ({})", host,
             ec.message(), ec.value());
        Disconnect(true);
        return;
    }

    _websocket.reset(
        new WebSocketStream_t(asio::make_strand(_ioContext), _sslContext));

    _websocket->next_layer().set_verify_mode(boost::asio::ssl::verify_none);
    auto insecure = true;
    _websocket->next_layer().set_verify_callback(
        [insecure](bool preverified, boost::asio::ssl::verify_context &ctx) {
            if (preverified) {
                return true;
            }
            if (insecure) {
                return true;
            }
            // X509 *cert = X509_STORE_CTX_get0_cert(ctx.native_handle());
            // STACK_OF(X509) *chain =
            // X509_STORE_CTX_get0_chain(ctx.native_handle()); return
            // SSLVerifier::VerifyX509(cert, chain);
            return true;
        });

    // 配置 SNI 设置
    if (!SSL_set_tlsext_host_name(_websocket->next_layer().native_handle(),
                                  host.c_str())) {
        boost::system::error_code ec{static_cast<int>(::ERR_get_error()),
                                     boost::asio::error::get_ssl_category()};
        OnSslHandshake(ec);
        return;
    }

    beast::get_lowest_layer(*_websocket)
        .expires_after(std::chrono::seconds(30));
    beast::get_lowest_layer(*_websocket)
        .async_connect(results,
                       beast::bind_front_handler(&WebSocket::OnConnect, this));
}

void WebSocket::OnConnect(
    beast::error_code ec,
    asio::ip::tcp::resolver::results_type::endpoint_type ep) {
    boost::ignore_unused(ep);

    logd("WebSocket::OnConnect");

    if (ec) {
        loge("Can't connect to server: {} ({})", ec.message(), ec.value());
        Disconnect(true);
        return;
    }

    beast::get_lowest_layer(*_websocket)
        .expires_after(std::chrono::seconds(30));
    _websocket->next_layer().async_handshake(
        asio::ssl::stream_base::client,
        beast::bind_front_handler(&WebSocket::OnSslHandshake, this));
}

void WebSocket::OnSslHandshake(beast::error_code ec) {
    logd("WebSocket::OnSslHandshake");

    if (ec) {
        loge("Can't establish secured connection to server: {} ({})",
             ec.message(), ec.value());
        Disconnect(true);
        return;
    }

    // websocket stream has its own timeout system
    beast::get_lowest_layer(*_websocket).expires_never();

    _websocket->set_option(beast::websocket::stream_base::timeout::suggested(
        beast::role_type::client));

    // set a decorator to change the User-Agent of the handshake
    _websocket->set_option(beast::websocket::stream_base::decorator(
        [](beast::websocket::request_type &req) {
            req.set(beast::http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " samp-discord-connector");
        }));

    _websocket->async_handshake(
        // host + ":443",
        host,
        //"/?encoding=json&v=10",
        path, beast::bind_front_handler(&WebSocket::OnHandshake, this));
}

void WebSocket::OnHandshake(beast::error_code ec) {
    logd("WebSocket::OnHandshake");

    if (ec) {
        loge("Can't upgrade to WSS protocol: {} ({})", ec.message(),
             ec.value());
        Disconnect(true);
        return;
    }

    _reconnectCount = 0;

    // read before identifying/resuming to make sure we catch the result
    Read();

    // 触发连接回调
    if (on_connect_callback_ != nullptr) {
        on_connect_callback_(this);
    }

    m_HeartbeatInterval = std::chrono::seconds(10);
    DoHeartbeat({});
}

void WebSocket::Disconnect(bool reconnect /*= false*/) {
    logd("WebSocket::Disconnect");

    _reconnect = reconnect;

    if (_websocket) {
        _websocket->async_close(
            beast::websocket::close_code::normal,
            beast::bind_front_handler(&WebSocket::OnClose, this));
    }
}

void WebSocket::OnClose(beast::error_code ec) {
    boost::ignore_unused(ec);

    logd("WebSocket::OnClose");

    m_HeartbeatTimer.cancel();

    if (_reconnect) {
        auto time = std::chrono::seconds(std::min(_reconnectCount * 5, 60u));

        if (time.count() > 0) {
            logi("reconnecting in {:d} seconds", time.count());
        }

        _reconnectTimer.expires_from_now(time);
        _reconnectTimer.async_wait(
            beast::bind_front_handler(&WebSocket::OnReconnect, this));
    }
}

void WebSocket::OnReconnect(beast::error_code ec) {
    logd("WebSocket::OnReconnect");

    if (ec) {
        switch (ec.value()) {
        case boost::asio::error::operation_aborted:
            // timer was chancelled, do nothing
            logd("reconnect timer chancelled");
            break;
        default:
            loge("reconnect timer error: {} ({})", ec.message(), ec.value());
            break;
        }
        return;
    }

    ++_reconnectCount;
    Connect();
}

void WebSocket::Read() {
    // logvd("WebSocket::Read");

    _websocket->async_read(
        _buffer, beast::bind_front_handler(&WebSocket::OnRead, this));
}

void WebSocket::OnRead(beast::error_code ec, std::size_t bytes_transferred) {
    // logd("WebSocket::OnRead({:d})", bytes_transferred);

    if (ec) {
        bool reconnect = false;
        switch (ec.value()) {
        case asio::ssl::error::stream_errors::stream_truncated:
            loge("Discord terminated websocket connection; reason: {} ({})",
                 _websocket->reason().reason.c_str(),
                 _websocket->reason().code);

            if (_websocket->reason().code == 4014) {
                printf(" >> discord-connector: bot could not connect due to "
                       "intent "
                       "permissions. Modify your discord bot settings and "
                       "enable every "
                       "intent.");
                reconnect = false;
            } else {
                reconnect = true;
            }
            break;
        case asio::error::operation_aborted:
            // connection was closed, do nothing
            break;
        default:
            loge("Can't read from Discord websocket gateway: {} ({})",
                 ec.message(), ec.value());
            reconnect = true;
            break;
        }

        if (reconnect) {
            logi("websocket gateway connection terminated; attempting "
                 "reconnect...");
            Disconnect(true);
        }
        return;
    }

    auto data = beast::buffers_to_string(_buffer.data());

    on_message_callback_(this, data.c_str(), data.length());

    _buffer.clear();

    Read();
}

void WebSocket::Write(std::string const &data) {
    // logvd("WebSocket::Write");

    _websocket->async_write(
        asio::buffer(data),
        beast::bind_front_handler(&WebSocket::OnWrite, this));
}

void WebSocket::OnWrite(beast::error_code ec, size_t bytes_transferred) {
    // logd("WebSocket::OnWrite({:d})", bytes_transferred);

    if (ec) {
        loge("Can't write to Discord websocket gateway: {} ({})", ec.message(),
             ec.value());

        // we don't handle reconnects here, as the read handler already does
        // this
    }
}

void WebSocket::DoHeartbeat(beast::error_code ec) {
    // logd("WebSocket::DoHeartbeat");

    if (ec) {
        switch (ec.value()) {
        case boost::asio::error::operation_aborted:
            // timer was chancelled, do nothing
            logd("heartbeat timer chancelled");
            break;
        default:
            loge("Heartbeat error: {} ({})", ec.message(), ec.value());
            break;
        }
        return;
    }

    // 触发心跳的方法
    if (on_heartbeat_callback_ != nullptr) {
        on_heartbeat_callback_(this);
    }

    m_HeartbeatTimer.expires_from_now(m_HeartbeatInterval);
    m_HeartbeatTimer.async_wait(
        beast::bind_front_handler(&WebSocket::DoHeartbeat, this));
}