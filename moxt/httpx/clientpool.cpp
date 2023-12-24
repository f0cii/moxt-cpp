//
// Created by ken on 2023/7/13.
//

#include "clientpool.hpp"
#include "libthread.hpp"
#include "moxt/utils/spscqueue_safe.hpp"
#include <photon/photon.h>
#include <photon/thread/thread-pool.h>
#include <photon/thread/thread.h>
#include <photon/thread/thread11.h>
#include <photon/thread/timer.h>
#include <photon/thread/workerpool.h>

using HttpClient = httpx::HttpClient;

class HttpClientPoolImpl : public HttpClientPool {
  public:
    using HttpClient = httpx::HttpClient;

    HttpClientPoolImpl(std::string host, std::string port,
                       ssl::context::method method);

    ~HttpClientPoolImpl() override;

    HttpClient *acquire();

    void release(HttpClient *client);

  private:
    SPSCQueueSafe<HttpClient *> pool_;
    SPSCQueueSafe<HttpClient *> closedPool_;

    int maxSize = 3;
    std::atomic<int> trackingCount{0};

    // 是否正在运行
    std::atomic<bool> running = false;

    HttpClient *createHttpClient();

    std::string host_;
    std::string port_;
    ssl::context::method method_ = ssl::context::tlsv13_client;

    photon::thread *collector = nullptr;
    // photon::Timer timer;
    void collect();
};

HttpClientPool *HttpClientPool::make(std::string host, std::string port,
                                     ssl::context::method method) {
    return new HttpClientPoolImpl(host, port, method);
}

// 初始化连接池
HttpClientPoolImpl::HttpClientPoolImpl(std::string host, std::string port,
                                       ssl::context::method method)
    : host_(host), port_(port), pool_(12), closedPool_(12), method_(method) {
    logd("HttpClientPoolImpl::HttpClientPoolImpl");
    collector = photon::thread_create11(&HttpClientPoolImpl::collect, this);
    seq_photon_thread_migrate_to_work_pool(collector);
}

HttpClientPoolImpl::~HttpClientPoolImpl() {
    logi("HttpClientPoolImpl::~HttpClientPoolImpl");
    if (collector != nullptr) {
        auto th = collector;
        collector = nullptr;
        photon::thread_interrupt((photon::thread *)th);
        photon::thread_join((photon::join_handle *)th);
    }
    logi("HttpClientPoolImpl::~HttpClientPoolImpl stop");
    // 清理连接池
    HttpClient *client = nullptr;
    while (pool_.pop(client)) {
        delete client;
    }
    while (closedPool_.pop(client)) {
        delete client;
    }
    logi("HttpClientPoolImpl::~HttpClientPoolImpl end");
}

void HttpClientPoolImpl::collect() {
    while (collector) {
        photon::thread_sleep(3);

        HttpClient *client = nullptr;
        if (closedPool_.pop(client)) {
            client->disconnect();
            delete client;
            client = nullptr;
        }
    }
}

// 获取连接
HttpClient *HttpClientPoolImpl::acquire() {
    // 从连接池中获取连接
    logd("HttpClientPoolImpl::acquire");
    HttpClient *client0 = nullptr;
    if (pool_.pop(client0)) {
        logd("HttpClientPoolImpl::acquire success (from pool)");
        trackingCount++;
        return client0;
    }
    // printf("HttpClientPoolImpl::acquire create...\n");
    auto client = createHttpClient();
    logd("HttpClientPoolImpl::acquire create httpclient success");
    if (client->connect()) {
        logd("Connect success");
        return client;
    }
    logw("HttpClientPoolImpl::acquire connect fail");
    return nullptr;
}

// 释放连接
void HttpClientPoolImpl::release(HttpClient *client) {
    if (client == nullptr) {
        return;
    }
    if (client->isConnected()) {
        pool_.push(client);
    } else {
        closedPool_.push(client);
    }
    trackingCount--;
}

HttpClient *HttpClientPoolImpl::createHttpClient() {
    logd("HttpClientPoolImpl::createHttpClient host={} port={}", host_, port_);

    // ssl::context ctx(ssl::context::tlsv12_client);
    // ssl::context ctx(ssl::context::tlsv13_client);

    ssl::context ctx(method_);

    ctx.set_default_verify_paths();

    auto client = new httpx::HttpClient(ctx, host_, port_);
    return client;
}