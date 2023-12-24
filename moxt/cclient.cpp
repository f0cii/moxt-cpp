#include "cclient.hpp"
#include "boost/beast/http/verb.hpp"
#include "fmt/format.h"
#include "httpx/httpbase.hpp"
#include "moxt/utils/url_utils.hpp"
#include "photon/net/curl.h"
#include <cstddef>
#include <cstdint>
#include <photon/photon.h>

// class StringStream {
//     std::string s;

//   public:
//     std::string &str() { return s; }

//     size_t write(void *c, size_t n) {
//         // LOG_DEBUG("CALL WRITE");
//         s.append((char *)c, n);
//         return n;
//     }
// };

CURLPool *new_curl_pool() {
    auto _ctor = [](photon::net::cURL **ptr) -> int {
        *ptr = new photon::net::cURL();
        return 0;
    };
    auto _dtor = [](photon::net::cURL *ptr) -> int {
        delete ptr;
        return 0;
    };

    auto *pool = new CURLPool(_ctor, _dtor);
    pool->enable_autoscale();
    return pool;
}

CClient::CClient(string baseUrl, ssl::context::method method)
    : baseUrl(baseUrl) {
    // logd("Client baseUrl={}", baseUrl);
    string host;
    string port;
    url_utils::parse_uri(baseUrl, host, port);

    this->host = host;

    curl_pool_ = new_curl_pool();
    // connPool = HttpClientPool::make(host, port, method);
    logd("Client.ctor");
}

CClient::~CClient() {
    // delete connPool;
    delete curl_pool_;
    logd("Client.~Client()");
}

CHttpResponse CClient::doRequest(const std::string &path, HttpVerb verb,
                                 std::map<std::string, std::string> &headers,
                                 const std::string &body, bool debug) {
    auto client = get_cURL();
    if (client == nullptr) {
        return CHttpResponse{400, "Failed to acquire client"};
    }
    DEFER(release_cURL(client));

    HttpRequest request_{verb, path, 11};

    // request_.set(http::field::host, host);
    // request_.set(http::field::user_agent, "xt/1.0.1");

    client->append_header("Host", host);
    client->append_header("User-Agent", "xt/1.0.1");

    // logd("--------------header--------------");
    for (auto &a : headers) {
        // logd("{}={}", a.first, a.second);
        // request_.set(a.first, a.second);
        client->append_header(a.first, a.second);
    }

    std::string request_url = "https://" + host + path;

    int timeout = 10;
    photon::net::StringWriter writer;
    long ret;
    if (body.length() > 0) {
        client->append_header("Content-Type", "application/json");
        client->append_header("Content-Length", fmt::to_string(body.size()));
        photon::net::BufReader body_(body.c_str(), body.length());

        ret = client->POST(request_url.c_str(), &body_, &writer,
                           (int64_t)timeout * 1000000);

        // request_.set(http::field::content_type, "application/json");
        // request_.set(http::field::content_length,
        // fmt::to_string(body.size())); request_.body() = body;
    } else {
        ret = client->GET(request_url.c_str(), &writer,
                          (int64_t)timeout * 1000000);
    }

    if (ret != 200) {
        // LOG_ERRNO_RETURN(0, -1, "connect to auth component failed. http
        // response code: `", ret);
        return CHttpResponse{
            500, fmt::format("请求失败 status={} body={}", ret, writer.string)};
    }

    return CHttpResponse{static_cast<uint64_t>(ret), writer.string};

    // auto client = connPool->acquire();
    // DEFER(connPool->release(client));
    // if (client == nullptr) {
    //     // logw("Failed to acquire client");
    //     return CHttpResponse{400, "Failed to acquire client"};
    // }
    // try {
    //     auto response = client->performRequestAsync(request_);
    //     int status_code = response.result_int();
    //     return CHttpResponse{status_code, response.body()};
    // } catch (std::exception &e) {
    //     loge("{}", e.what());
    //     return CHttpResponse{500, e.what()};
    // } catch (...) {
    //     loge("Error");
    //     return CHttpResponse{500, "error"};
    // }
}

SEQ_FUNC CClient *seq_cclient_new(const char *baseUrl, size_t baseUrl_len,
                                  int64_t method) {
    auto method_ = static_cast<ssl::context::method>(method);
    return new CClient(string(baseUrl, baseUrl_len), method_);
}

SEQ_FUNC void seq_cclient_free(CClient *client) {
    delete client;
    client = nullptr;
}

SEQ_FUNC int64_t seq_cclient_do_request(
    CClient *client, const char *path, size_t path_len, int64_t verb,
    std::map<std::string, std::string> *headers, const char *body,
    size_t body_len, char *res, size_t *n) {
    logd("seq_client_do_request");
    std::string reqeustPath = std::string(path, path_len);
    std::string body_(body, body_len);
    HttpVerb v = HttpVerb::get;
    auto verb_ = static_cast<boost::beast::http::verb>(verb);
    if (verb_ == boost::beast::http::verb::get) {
        v = HttpVerb::get;
    } else if (verb_ == boost::beast::http::verb::post) {
        v = HttpVerb::post;
    } else if (verb_ == boost::beast::http::verb::put) {
        v = HttpVerb::put;
    } else if (verb_ == boost::beast::http::verb::delete_) {
        v = HttpVerb::delete_;
    }
    auto result = client->doRequest(reqeustPath, v, *headers, body_, false);
    logd("seq_client_do_request success");
    auto &res_body = result.body;
    memcpy(res, res_body.c_str(), res_body.length());
    *n = res_body.length();
    return result.status_code;
}