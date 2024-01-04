#ifndef MOXT_CCLIENT_HPP
#define MOXT_CCLIENT_HPP

#include "common.hpp"
#include "moxt/httpx/clientpool.hpp"
#include "moxt/httpx/httpbase.hpp"
#include "moxt/httpx/httpclient.hpp"
#include <photon/common/identity-pool.h>
#include <photon/net/curl.h>
#include <photon/net/http/verb.h>
#include <photon/photon.h>

using namespace std;

typedef IdentityPool<photon::net::cURL, 10> CURLPool;

constexpr int64_t VERB_UNKNOWN = 0;
constexpr int64_t VERB_DELETE = 1;
constexpr int64_t VERB_GET = 2;
constexpr int64_t VERB_HEAD = 3;
constexpr int64_t VERB_POST = 4;
constexpr int64_t VERB_PUT = 5;

class CClient {
  public:
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="baseUrl"></param>
    /// <param name="method"></param>
    explicit CClient(string baseUrl, ssl::context::method method);

    ~CClient();

    CHttpResponse doRequest(const std::string &path,
                            photon::net::http::Verb verb,
                            std::map<std::string, std::string> &headers,
                            const std::string &body, bool debug);

    CHttpResponse doRequestTest(const std::string &path,
                                photon::net::http::Verb verb,
                                std::map<std::string, std::string> &headers,
                                const std::string &body, bool debug);

  private:
    photon::net::cURL *get_cURL();

    void release_cURL(photon::net::cURL *curl);
    string baseUrl;
    string host;
};

void init_curl_pool();

SEQ_FUNC CClient *seq_cclient_new(const char *baseUrl, size_t baseUrl_len,
                                  int64_t method);
SEQ_FUNC void seq_cclient_free(CClient *client);

SEQ_FUNC int64_t seq_cclient_do_request(
    CClient *client, const char *path, size_t path_len, int64_t verb,
    std::map<std::string, std::string> *headers, const char *body,
    size_t body_len, char *res, size_t *n, bool verbose);

#endif