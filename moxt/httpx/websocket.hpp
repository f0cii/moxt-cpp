#pragma once

#include "moxt/common.hpp"
#include "moxt/httpx/httpbase.hpp"
#include "moxt/std23/function_ref.h"
#include <functional>

// 参考以下代码实现:
namespace asio = boost::asio;
namespace beast = boost::beast;

class WebSocket;

// using std23::function_ref;
// using std23::nontype;

// typedef void (*OnConnectCallback)(WebSocket *ws);
// typedef void (*OnHeartbeatCallback)(WebSocket *ws);
// typedef void (*OnMessageCallback)(WebSocket *ws, const char *data, size_t
// len);

class WebSocket;

struct WebSocketHeaders {
    std::map<std::string, std::string> headers;

    void set(const std::string &key, const std::string &value) {
        headers[key] = value;
    }
};

typedef std23::function_ref<void(WebSocket *ws)> OnConnectedCallback;
typedef std23::function_ref<void(WebSocket *ws)> BeforeReconnectCallback;
typedef std23::function_ref<void(WebSocket *ws)> OnHeartbeatCallback;
typedef std23::function_ref<void(WebSocket *ws, const char *data, size_t len)>
    OnMessageCallback;

class WebSocket {
  public:
    ~WebSocket();

  private:
    const int LARGE_THRESHOLD_NUMBER = 100;

    asio::io_context &_ioContext;
    asio::ip::tcp::resolver _resolver;
    asio::ssl::context _sslContext;
    using SslStream_t = beast::ssl_stream<beast::tcp_stream>;
    using WebSocketStream_t = beast::websocket::stream<SslStream_t>;
    std::unique_ptr<WebSocketStream_t> _websocket;

    bool _reconnect = false;
    asio::steady_timer _reconnectTimer;
    unsigned int _reconnectCount = 0;

    beast::multi_buffer _buffer;

    std::string host;
    std::string port;
    std::string path;
    uint64_t _sequenceNumber = 0;
    std::string m_SessionId;
    asio::steady_timer m_HeartbeatTimer;
    std::chrono::steady_clock::duration m_HeartbeatInterval;

    WebSocketHeaders currentHeaders_;                 // 当前的 headers
    BeforeReconnectCallback beforeReconnectCallback_; // 重连前的回调
    OnConnectedCallback on_connected_callback_;
    OnMessageCallback on_message_callback_;
    OnHeartbeatCallback on_heartbeat_callback_;

  private:
    void OnResolve(beast::error_code ec,
                   asio::ip::tcp::resolver::results_type results);
    void OnConnect(beast::error_code ec,
                   asio::ip::tcp::resolver::results_type::endpoint_type ep);
    void OnSslHandshake(beast::error_code ec);
    void OnHandshake(beast::error_code ec);
    void SetHeaders(beast::websocket::request_type &req);

    void OnClose(beast::error_code ec);
    void OnReconnect(beast::error_code ec);

    void Read();
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);

    void OnWrite(beast::error_code ec, size_t bytes_transferred);

    void DoHeartbeat(beast::error_code ec);

  public:
    WebSocket(std::string host, std::string port, std::string path,
              asio::io_context &ioContext, int tls_version = TLS1_3_VERSION);
    void Connect();
    void Disconnect(bool reconnect = false);
    void Write(std::string const &data);

    // 设置Headers
    void SetHeaders(const std::map<std::string, std::string> &headers) {
        currentHeaders_.headers = headers;
    }

    // 设置连接回调函数
    void SetOnConnected(OnConnectedCallback callback) {
        on_connected_callback_ = callback;
    }

    void SetBeforeReconnectCallback(BeforeReconnectCallback callback) {
        beforeReconnectCallback_ = callback;
    }

    // 设置心跳回调函数
    void SetOnHeartbeat(OnHeartbeatCallback callback) {
        on_heartbeat_callback_ = callback;
    }

    // 设置消息回调函数
    void SetOnMessage(OnMessageCallback callback) {
        on_message_callback_ = callback;
    }
};