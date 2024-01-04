#include "cclient.hpp"
#include "boost/beast/http/verb.hpp"
#include "fmt/format.h"
#include "httpx/httpbase.hpp"
#include "moxt/utils/url_utils.hpp"
#include <cstddef>
#include <cstdint>

CURLPool *new_curl_pool() {
    auto _ctor = [](photon::net::cURL **ptr) -> int {
        *ptr = new photon::net::cURL();
        logi("new photon::net::cURL()");
        return 0;
    };
    auto _dtor = [](photon::net::cURL *ptr) -> int {
        delete ptr;
        logi("delete photon::net::cURL");
        return 0;
    };

    auto *pool = new CURLPool(_ctor, _dtor);
    pool->enable_autoscale();
    return pool;
}

static CURLPool *curl_pool_ = nullptr;

void init_curl_pool() { curl_pool_ = new_curl_pool(); }

CClient::CClient(string baseUrl, ssl::context::method method)
    : baseUrl(baseUrl) {
    string host;
    string port;
    url_utils::parse_uri(baseUrl, host, port);

    this->host = host;
}

CClient::~CClient() {}

photon::net::cURL *CClient::get_cURL() {
    auto curl = curl_pool_->get();
    curl->reset_error();
    curl->reset().clear_header();
    // curl->setopt(CURLOPT_SSL_VERIFYHOST, 0L);
    // curl->setopt(CURLOPT_SSL_VERIFYPEER, 0L);
    return curl;
};

void CClient::release_cURL(photon::net::cURL *curl) { curl_pool_->put(curl); };

CHttpResponse CClient::doRequest(const std::string &path,
                                 photon::net::http::Verb verb,
                                 std::map<std::string, std::string> &headers,
                                 const std::string &body, bool debug) {
    auto client = get_cURL();
    if (client == nullptr) {
        return CHttpResponse{400, "Failed to acquire client"};
    }
    DEFER(release_cURL(client));

    photon::net::HeaderMap resHeaders;
    if (debug) {
        client->set_verbose(true);
        client->set_header_container(&resHeaders);
    }

    client->append_header("Host", host);
    client->append_header("User-Agent", "xt/1.0.1");

    // logd("--------------header--------------");
    for (auto &a : headers) {
        // logd("{}={}", a.first, a.second);
        client->append_header(a.first, a.second);
    }

    std::string request_url = "https://" + host + path;

    int64_t timeout = 5;
    photon::net::StringWriter writer;
    long ret;
    if (verb == photon::net::http::Verb::DELETE) {
        ret = client->DELETE(request_url.c_str(), &writer, timeout * 1000000);
    } else if (verb == photon::net::http::Verb::GET) {
        ret = client->GET(request_url.c_str(), &writer, timeout * 1000000);
    } else if (verb == photon::net::http::Verb::HEAD) {
        ret = client->HEAD(request_url.c_str(), &writer, timeout * 1000000);
    } else if (verb == photon::net::http::Verb::POST) {
        // client->append_header("Content-Type", "application/json");
        // client->append_header("Content-Length", fmt::to_string(body.size()));
        photon::net::BufReader body_(body.c_str(), body.length());

        ret = client->POST(request_url.c_str(), &body_, &writer,
                           timeout * 1000000);
    } else if (verb == photon::net::http::Verb::PUT) {
        // client->append_header("Content-Type", "application/json");
        // client->append_header("Content-Length", fmt::to_string(body.size()));
        photon::net::BufReader body_(body.c_str(), body.length());

        ret = client->PUT(request_url.c_str(), &body_, &writer,
                          timeout * 1000000);
    } else {
        ret = client->GET(request_url.c_str(), &writer, timeout * 1000000);
    }

    if (debug) {
        // logd("response headers: ");
        for (auto i : resHeaders) {
            logd("{}={}", i.first, i.second);
        }
    }

    if (ret != 200) {
        logw("connect to server failed. http response code: {}", ret);
        return CHttpResponse{
            500, fmt::format("请求失败 status={} body={}", ret, writer.string)};
    }

    return CHttpResponse{static_cast<uint64_t>(ret), writer.string};
}

CHttpResponse
CClient::doRequestTest(const std::string &path, photon::net::http::Verb verb,
                       std::map<std::string, std::string> &headers,
                       const std::string &body, bool debug) {
    auto client = get_cURL();
    if (client == nullptr) {
        return CHttpResponse{400, "Failed to acquire client"};
    }
    DEFER(release_cURL(client));

    client->set_verbose(true);
    photon::net::HeaderMap resHeaders;
    client->set_header_container(&resHeaders);

    client->append_header("Host", host);
    client->append_header("User-Agent", "xt/1.0.1");

    // logd("--------------header--------------");
    for (auto &a : headers) {
        // logd("{}={}", a.first, a.second);
        client->append_header(a.first, a.second);
    }

    std::string request_url = "https://" + host + path;

    int64_t timeout = 5;
    photon::net::StringWriter writer;
    // photon::net::DummyReaderWriter writer; // dummy
    long ret;
    if (verb == photon::net::http::Verb::DELETE) {
        ret = client->DELETE(request_url.c_str(), &writer, timeout * 1000000);
    } else if (verb == photon::net::http::Verb::GET) {
        ret = client->GET(request_url.c_str(), &writer, timeout * 1000000);
    } else if (verb == photon::net::http::Verb::HEAD) {
        ret = client->HEAD(request_url.c_str(), &writer, timeout * 1000000);
    } else if (verb == photon::net::http::Verb::POST) {
        // client->append_header("Content-Type", "application/json");
        // client->append_header("Content-Length", fmt::to_string(body.size()));
        photon::net::BufReader body_(body.c_str(), body.length());

        ret = client->POST(request_url.c_str(), &body_, &writer,
                           timeout * 1000000);
    } else if (verb == photon::net::http::Verb::PUT) {
        // client->append_header("Content-Type", "application/json");
        // client->append_header("Content-Length", fmt::to_string(body.size()));
        photon::net::BufReader body_(body.c_str(), body.length());

        ret = client->PUT(request_url.c_str(), &body_, &writer,
                          timeout * 1000000);
    } else {
        ret = client->GET(request_url.c_str(), &writer, timeout * 1000000);
    }

    logi("response headers: ");
    for (auto i : resHeaders) {
        logi("{}={}", i.first, i.second);
    }

    if (ret != 200) {
        logw("connect to server failed. http response code: {}", ret);
        return CHttpResponse{
            500, fmt::format("请求失败 status={} body={}", ret, writer.string)};
    }

    return CHttpResponse{static_cast<uint64_t>(ret), writer.string};
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
    size_t body_len, char *res, size_t *n, bool verbose) {
    // logd("seq_cclient_do_request");
    std::string reqeustPath = std::string(path, path_len);
    std::string body_(body, body_len);
    photon::net::http::Verb v = photon::net::http::Verb::GET;
    if (verb == VERB_DELETE) {
        v = photon::net::http::Verb::DELETE;
    } else if (verb == VERB_GET) {
        v = photon::net::http::Verb::GET;
    } else if (verb == VERB_HEAD) {
        v = photon::net::http::Verb::HEAD;
    } else if (verb == VERB_POST) {
        v = photon::net::http::Verb::POST;
    } else if (verb == VERB_PUT) {
        v = photon::net::http::Verb::PUT;
    }
    auto result = client->doRequest(reqeustPath, v, *headers, body_, verbose);
    // logd("seq_cclient_do_request success");
    auto &res_body = result.body;
    memcpy(res, res_body.c_str(), res_body.length());
    *n = res_body.length();
    return result.status_code;
}