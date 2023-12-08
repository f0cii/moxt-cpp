#include "client.hpp"
#include "absl/container/internal/compressed_tuple.h"
#include "moxt/utils/url_utils.hpp"

Client::Client(string baseUrl, ssl::context::method method) : baseUrl(baseUrl) {
    // logd("Client baseUrl={}", baseUrl);
    string host;
    string port;
    url_utils::parse_uri(baseUrl, host, port);

    this->host = host;

    // connPool = HttpClientPool::make(host, port, ssl::context::tlsv12_client);
    connPool = HttpClientPool::make(host, port, method);
    logd("Client.ctor");
}

Client::~Client() {
    delete connPool;
    logd("Client.~Client()");
}

inline void set_body(char *res, const char *s, size_t len) {
    memcpy(res, s, len);
    res[len] = '\0';
}

int64_t Client::doRequest(const char *path, size_t path_len, int64_t verb,
                          std::map<std::string, std::string> *headers,
                          const char *body, size_t body_len, char *res,
                          size_t *n, bool debug) {
    auto path_ = std::string(path, path_len);
    auto verb_ = static_cast<HttpVerb>(verb);

    HttpRequest request_{verb_, path_, 11};

    request_.set(http::field::host, host);
    request_.set(http::field::user_agent, "ct/1.0.1");

    // request_.set(http::field::content_type, "application/json");
    // if (body_len > 0) {
    //     if (debug) {
    //         logd("body_len={}", body_len);
    //         logd("body={}", std::string(body, body_len));
    //     }
    //     // request_.set(http::field::content_length,
    //     fmt::to_string(body_len));
    //     // request_.body() = std::string(body, body_len);
    // }

    if (body_len > 0) {
        request_.set(http::field::content_type, "application/json");
        std::string body_(body, body_len);
        request_.set(http::field::content_length, fmt::to_string(body_.size()));
        request_.body() = body_;
    }

    if (headers != nullptr) {
        // printf("doRequest 4\n");
        logd("--------------header--------------");
        for (auto &a : *headers) {
            logd("{}={}", a.first, a.second);
            request_.set(a.first, a.second);
        }
    }
    // if (body_len > 0) {
    //     logd("body_len={}", body_len);
    //     logd("body={}", std::string(body, body_len));
    //     request_.set(http::field::content_length, fmt::to_string(body_len));
    //     request_.body() = std::string(body, body_len);
    // }

    // printf("Client::doRequest 10000\n");

    auto client = connPool->acquire();
    if (client == nullptr) {
        // printf("Client::doRequest 10001\n");
        // printf("doRequest 5\n");
        // 构造错误响应结果
        const char *msg = "Failed to acquire client";
        auto len = strlen(msg);
        set_body(res, msg, len);
        *n = len;
        return 404;
    }

    // printf("Client::doRequest 10002\n");

    size_t bufferSize = 0; // 1024*64;
    auto response = client->performRequest(request_, bufferSize);
    // printf("Client::doRequest 10003\n");
    connPool->release(client);
    // printf("Client::doRequest 10004\n");

    auto resBody = &response.body();

    auto len = resBody->length();

    // printf("Client::doRequest status: %d\n", response.result_int());
    // printf("Client::doRequest body: %s\n", resBody->c_str());
    // printf("Client::doRequest len: %ld\n", len);

    set_body(res, resBody->c_str(), len);
    // printf("Client::doRequest set_body\n");
    *n = len;
    // printf("Client::doRequest set n\n");
    return response.result_int();
}

SEQ_FUNC Client *seq_client_new(const char *baseUrl, size_t baseUrl_len,
                                int64_t method) {
    auto method_ = static_cast<ssl::context::method>(method);
    // if (method_ == ssl::context::tlsv12_client) {
    //     printf("seq_client_new tlsv12_client\n");
    // } else if (method == ssl::context::tlsv13_client) {
    //     printf("seq_client_new tlsv13_client\n");
    // } else {
    //     printf("seq_client_new unknown\n");
    // }
    return new Client(string(baseUrl, baseUrl_len), method_);
}

SEQ_FUNC void seq_client_free(Client *client) {
    delete client;
    client = nullptr;
}

SEQ_FUNC int64_t seq_client_do_request(Client *client, SeqHttpRequest *reqeust,
                                       char *res, size_t *n) {
    // printf("seq_client_do_request\n");
    std::string path(reqeust->path, reqeust->path_len);
    // printf("seq_client_do_request path=%s\n", path.c_str());
    // printf("seq_client_do_request verb=%ld\n", reqeust->verb);
    if (reqeust->body_len > 0) {
        std::string body(reqeust->body, reqeust->body_len);
        // printf("seq_client_do_request body=%s\n", body.c_str());
        // printf("seq_client_do_request body_len=%ld\n", reqeust->body_len);
    }
    // const char *a =
    //     "trings_internal -labsl_flags_program_name -labsl_int128 "
    //     "-labsl_scoped_set_env -labsl_raw_hash_set "
    //     "-labsl_random_internal_seed_material -labsl_random_internal_randen "
    //     "-labsl_random_internal_randen_slow "
    //     "-labsl_random_internal_randen_hwaes_impl "
    //     "-labsl_random_internal_randen_hwaes -labsl_graphcycles_internal "
    //     "-labsl_exponential_biased -labsl_bad_variant_access -labsl_statusor
    //     "
    //     "-labsl_random_internal_distribution_test_util "
    //     "-labsl_random_internal_platform -labsl_hashtablez_sampler "
    //     "-labsl_demangle_internal -labsl_leak_check -labsl_log_severity "
    //     "-labsl_raw_logging_internal -labsl_strerror -labsl_examine_stack "
    //     "-labsl_low_level_hash -labsl_random_seed_gen_exception "
    //     "-labsl_civil_time -labsl_crc_cord_state -labsl_crc32c "
    //     "-labsl_crc_cpu_detect -labsl_crc_internal -lsimdj";
    // printf("seq_client_do_request a\n");
    // strcpy(res, a);
    logi("seq_client_do_request");
    auto result = client->doRequest(
        reqeust->path, reqeust->path_len, reqeust->verb,
        static_cast<std::map<std::string, std::string> *>(reqeust->headers),
        reqeust->body, reqeust->body_len, res, n, reqeust->debug);
    logi("seq_client_do_request success");
    return result;
}

SEQ_FUNC void seq_c_free(char *res) {
    if (res != nullptr) {
        free(res);
    }
    res = nullptr;
}
