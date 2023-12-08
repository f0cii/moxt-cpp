#ifndef __CTCLIENT_H__
#define __CTCLIENT_H__

#include "common.hpp"
#include "moxt/httpx/clientpool.hpp"
#include "moxt/httpx/httpbase.hpp"
#include "moxt/httpx/httpclient.hpp"
#include <cstdint>

using namespace std;

typedef std::chrono::system_clock Clock;

class Client {
  public:
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="baseUrl"></param>
    /// <param name="method"></param>
    explicit Client(string baseUrl, ssl::context::method method);

    ~Client();

    // 执行请求的统一方法
    int64_t doRequest(const char *path, size_t path_len, int64_t verb,
                      std::map<std::string, std::string> *headers,
                      const char *body, size_t body_len, char *res, size_t *n,
                      bool debug = false);

  private:
    string baseUrl;
    string host;
    HttpClientPool *connPool;
};

struct SeqHttpRequest {
    const char *path;
    size_t path_len;
    int64_t verb;
    void *headers; // std::map<std::string, std::string>
    const char *body;
    size_t body_len;
    bool debug;
};

SEQ_FUNC Client *seq_client_new(const char *baseUrl, size_t baseUrl_len,
                                int64_t method);
SEQ_FUNC void seq_client_free(Client *client);

// const char *path, int64_t verb, std::map<std::string, std::string> *headers,
// const char *body, size_t body_len
SEQ_FUNC int64_t seq_client_do_request(Client *client, SeqHttpRequest *reqeust,
                                       char *res, size_t *n);
SEQ_FUNC void seq_c_free(char *res);

#endif