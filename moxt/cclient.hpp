#ifndef MOXT_CCLIENT_HPP
#define MOXT_CCLIENT_HPP

#include "common.hpp"
#include "moxt/client.hpp"
#include "moxt/httpx/clientpool.hpp"
#include "moxt/httpx/httpbase.hpp"
#include "moxt/httpx/httpclient.hpp"
#include <photon/common/identity-pool.h>
#include <photon/net/curl.h>

using namespace std;

typedef IdentityPool<photon::net::cURL, 10> CURLPool;

class CClient {
  public:
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="baseUrl"></param>
    /// <param name="method"></param>
    explicit CClient(string baseUrl, ssl::context::method method);

    ~CClient();

    // CHttpResponse doRequestSync(const std::string &path, HttpVerb verb,
    //                         std::map<std::string, std::string> &headers,
    //                         const std::string &body, bool debug);

    CHttpResponse doRequest(const std::string &path, HttpVerb verb,
                            std::map<std::string, std::string> &headers,
                            const std::string &body, bool debug);

    // // 执行请求的统一方法
    // int64_t doRequest(const char *path, size_t path_len, int64_t verb,
    //                   std::map<std::string, std::string> *headers,
    //                   const char *body, size_t body_len, char *res, size_t
    //                   *n, bool debug = false);

  private:
    photon::net::cURL *get_cURL() {
        auto curl = curl_pool_->get();
        curl->reset_error();
        curl->reset().clear_header();
        return curl;
    };

    void release_cURL(photon::net::cURL *curl) { curl_pool_->put(curl); };
    string baseUrl;
    string host;
    CURLPool *curl_pool_;
};

SEQ_FUNC CClient *seq_cclient_new(const char *baseUrl, size_t baseUrl_len,
                                  int64_t method);
SEQ_FUNC void seq_cclient_free(CClient *client);

SEQ_FUNC int64_t seq_cclient_do_request(
    CClient *client, const char *path, size_t path_len, int64_t verb,
    std::map<std::string, std::string> *headers, const char *body,
    size_t body_len, char *res, size_t *n);

#endif