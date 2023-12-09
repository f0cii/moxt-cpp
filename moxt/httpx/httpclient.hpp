#ifndef __HTTPX_H__
#define __HTTPX_H__

//#include "common.hpp"
#include <atomic>
#include <optional>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include "libthread.hpp"
#include <photon/photon.h>
#include <photon/thread/thread-pool.h>
#include <photon/thread/thread.h>
#include <photon/thread/thread11.h>
#include <photon/thread/timer.h>
#include <photon/thread/workerpool.h>

#include "moxt/httpx/asio_ioc.hpp"

using namespace std::chrono_literals;
namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = net::ssl;

using net::ip::tcp;

namespace httpx {
using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;

/// <summary>
/// HTTP响应数据
/// </summary>
struct ResData {
    /// <summary>
    /// 状态码
    /// </summary>
    unsigned int status;

    /// <summary>
    /// 内容
    /// </summary>
    std::string body;
};

/// <summary>
/// HTTP客户端
/// </summary>
class HttpClient {
    // net::any_io_executor *ex_;
    ssl::context &ctx_;
    std::string host_;
    std::string port_;
    std::optional<beast::ssl_stream<beast::tcp_stream>> ssl_stream_;
    beast::flat_buffer buffer_;
    // 连接超时时间
    std::chrono::milliseconds connectTimeout_{5000};
    // 请求超时时间
    std::chrono::milliseconds requestTimeout_{5000};
    std::atomic<bool> connected = false;

    static constexpr auto kTimeout = 3s;

  public:
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="ctx"></param>
    /// <param name="host"></param>
    /// <param name="port"></param>
    HttpClient(ssl::context &ctx, std::string &host, std::string &port)
        : ctx_(ctx), host_(host), port_(port),
          ssl_stream_(std::in_place, AsioIOC::default_pool().getIOContext(),
                      ctx_) {
        logi("HttpClient::HttpClient()");
    }

    /// <summary>
    /// 析构函数
    /// </summary>
    ~HttpClient() {
        logi("HttpClient::~HttpClient()");
        if (connected) {
            disconnect();
        }
        logi("HttpClient::~HttpClient() end");
    }

    /// <summary>
    /// 是否已连接
    /// </summary>
    inline bool isConnected() { return connected; }

    /// <summary>
    /// 连接
    /// </summary>
    bool connect() {
        connected = false;

        logd("client.connect {}:{}", host_, port_);

        // Set a timeout on the operation
        beast::get_lowest_layer(*ssl_stream_).expires_after(connectTimeout_);

        auto ep = tcp::resolver(AsioIOC::default_pool().getIOContext())
                      .resolve(host_, port_); // "https"

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if (!SSL_set_tlsext_host_name(ssl_stream_->native_handle(),
                                      host_.data())) {
            // beast::error_code ec{static_cast<int>(::ERR_get_error()),
            // net::error::get_ssl_category()}; throw beast::system_error{ec};
            printf("Failed to set SNI Hostname for %s\n", host_.c_str());
            loge("Failed to set SNI Hostname for {}", host_.c_str());
            return false;
        }

        beast::error_code ec;
        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(*ssl_stream_).connect(ep, ec);
        if (ec) {
            printf("Failed to connect to %s for %s\n", ep->host_name().c_str(),
                   ec.message().c_str());
            loge("Failed to connect to {} for {}", ep->host_name(),
                 ec.message());
            return false;
        }

        ssl_stream_->handshake(ssl::stream_base::client, ec);
        if (ec) {
            printf("Failed to handshake to %s for %s", ep->host_name().c_str(),
                   ec.message().c_str());
            loge("Failed to handshake to {} for {}", ep->host_name(),
                 ec.message());
            return false;
        }

        connected = true;
        return true;
    }

    /// <summary>
    /// 断开连接
    /// </summary>
    void disconnect() {
        beast::error_code ec;
        connected = false;
        ssl_stream_->shutdown(ec);
        if (ec) {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec = {};
        }
        if (ec)
            throw boost::beast::system_error{ec};
    }

    /// <summary>
    ///
    /// </summary>
    /// <param name="timeout"></param>
    inline void SetTimeout(std::chrono::seconds timeout) {
        get_lowest_layer(*ssl_stream_).expires_after(timeout);
    }

    /// <summary>
    ///
    /// </summary>
    /// <param name="size"></param>
    void reserveBufferSize(size_t size) { buffer_.reserve(size); }

    /// <summary>
    /// 计算缓冲区大小
    /// </summary>
    /// <param name="sizeInKB"></param>
    /// <returns></returns>
    constexpr static int computeBufferSize(int sizeInKB) {
        return sizeInKB * 1024; // convert size from KB to bytes
    }

    /// <summary>
    /// HTTP请求
    /// </summary>
    /// <param name="request"></param>
    /// <param name="bufferSize"></param>
    /// <returns></returns>
    inline Response performRequest(const Request &request,
                                   size_t bufferSize = 0) {
        if (!connected) {
            if (connect()) {
                logi("Successfully connected to {} for {}",
                     ssl_stream_->next_layer()
                         .socket()
                         .remote_endpoint()
                         .address()
                         .to_string(),
                     host_);
            } else {
                loge("Failed to connect to {}", host_);
                throw std::runtime_error("Failed to connect to " + host_ + ":" +
                                         port_);
            }
        }

        // get_lowest_layer(*ssl_stream_).expires_after(requestTimeout_);

        beast::error_code ec;
        beast::http::write(*ssl_stream_, request, ec);
        if (ec) {
            loge("Failed to write request for {}", ec.message());
            connected = false;
            // using Response = http::response<http::string_body>;
            Response response;
            response.result(beast::http::status::not_found);
            return response;
        }

        Response response;

        if (bufferSize > 0) {
            beast::flat_buffer buffer;
            buffer.reserve(bufferSize);
            beast::http::read(*ssl_stream_, buffer, response, ec);
        } else {
            buffer_.consume(buffer_.size());
            beast::http::read(*ssl_stream_, buffer_, response, ec);
        }

        if (ec) {
            loge("Failed to read response for {}", ec.message());
            response.result(beast::http::status::not_found);
            connected = false;
        }

        // return std::move(response);
        return response;
    }

    /// <summary>
    /// 异步HTTP请求
    /// </summary>
    Response performRequestAsync(const Request &request) {
        auto &s = *ssl_stream_;
        // get_lowest_layer(s).expires_after(requestTimeout_);

        photon::condition_variable writeCompleted;
        std::pair<boost::system::error_code, size_t> writeResult;
        auto writeCallback = [&writeCompleted,
                              &writeResult](auto ec, auto size) mutable {
            writeResult = std::make_pair(ec, size);
            writeCompleted.notify_one();
        };
        http::async_write(s, request, writeCallback);
        writeCompleted.wait_no_lock(1000 * 1000 * 10); // 5s us

        // Check for errors in the first async_write
        if (writeResult.first) {
            // Handle error, log, set error state, etc.
            throw std::runtime_error("Error in async_write: " +
                                     writeResult.first.message());
        }

        buffer_.consume(buffer_.size());

        photon::condition_variable readCompleted;
        Response res;
        std::pair<boost::system::error_code, size_t> readResult;

        auto readCallback = [&readCompleted, &readResult](auto ec,
                                                          auto size) mutable {
            readResult = std::make_pair(ec, size);
            readCompleted.notify_one();
        };
        http::async_read(s, buffer_, res, readCallback);
        readCompleted.wait_no_lock(1000 * 1000 * 10);

        // Check for errors in the async_read
        if (readResult.first) {
            // Handle error, log, set error state, etc.
            throw std::runtime_error("Error in async_read: " +
                                     readResult.first.message());
        }

        return res;
    }
};

} // namespace httpx

#endif