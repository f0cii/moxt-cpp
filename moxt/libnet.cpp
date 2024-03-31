#include "libnet.hpp"

#include "common.hpp"
#include "libthread.hpp"
#include "moxt/httpx/asio_ioc.hpp"
#include "moxt/httpx/httpbase.hpp"
#include "moxt/httpx/httpclient.hpp"
#include "moxt/httpx/websocket.hpp"
#include <photon/net/curl.h>
#include <photon/net/http/client.h>
#include <photon/net/http/message.h>
#include <photon/net/http/verb.h>
#include <photon/photon.h>
#include <photon/thread/thread-pool.h>
#include <photon/thread/thread.h>
#include <photon/thread/thread11.h>
#include <photon/thread/workerpool.h>
#include <time.h>
#include <type_traits>

using namespace photon::net;
using namespace photon::net::http;

SEQ_FUNC void seq_asio_run() { AsioIOC::default_pool().run(); }

SEQ_FUNC void seq_asio_poll() { AsioIOC::default_pool().poll(); }

SEQ_FUNC void seq_asio_poll_one() { AsioIOC::default_pool().poll_one(); }

SEQ_FUNC void seq_asio_stop() { AsioIOC::default_pool().stop(); }

SEQ_FUNC WebSocket *seq_websocket_new(const char *host, const char *port,
                                      const char *path, int tls_version) {
    auto &ioc = AsioIOC::default_pool().getIOContext();
    auto ws = new WebSocket(host, port, path, ioc, tls_version);
    return ws;
}

SEQ_FUNC void seq_websocket_delete(WebSocket *p) {
    delete p;
    p = nullptr;
}

SEQ_FUNC void seq_websocket_connect(WebSocket *p) { p->Connect(); }

SEQ_FUNC void seq_websocket_disconnect(WebSocket *p) { p->Disconnect(); }

SEQ_FUNC void seq_websocket_send(WebSocket *p, const char *text, size_t len) {
    auto data = std::string(text, len);
    p->Write(data);
}

SEQ_FUNC void seq_websocket_set_on_connect(WebSocket *p,
                                           OnConnectCallback_t callback) {
    p->set_on_connect(callback);
}

SEQ_FUNC void seq_websocket_set_on_heartbeat(WebSocket *p,
                                             OnHeartbeatCallback_t callback) {
    p->set_on_heartbeat(callback);
}

SEQ_FUNC void seq_websocket_set_on_message(WebSocket *p,
                                           OnMessageCallback_t callback) {
    p->set_on_message(callback);
}

SEQ_FUNC AsioIOC *seq_asio_ioc() { return &AsioIOC::default_pool(); }

SEQ_FUNC void seq_asio_ioc_poll(AsioIOC *ioc) { ioc->poll(); }

void *coro_net_run(void *arg) {
    auto &pool = AsioIOC::default_pool();
    for (;;) {
        pool.poll();
        photon::thread_yield();
        // photon::thread_usleep(0);
    }
}

void thread_net_run() {
    int ret = photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_DEFAULT);
    logi("thread_net_run");
    AsioIOC::default_pool().run();
    logi("thread_net_run stopped");
}

SEQ_FUNC void seq_init_net(int type) {
    if (type == STD_THREAD) {
        auto bgThread = new std::thread(thread_net_run);
    } else if (type == PHOTON) {
        auto work_pool = seq_photon_work_pool();

        if (work_pool == nullptr) {
            logw("Error: Work pool is nullptr in seq_init_net");
            return;
        }

        work_pool->thread_migrate(photon::thread_create(coro_net_run, nullptr),
                                  -1UL);
    } else {
        //
    }
}

const size_t std_data_size = 64 * 1024;

void req(Client *client, const char *target) {
    auto op_test = client->new_operation(Verb::GET, target);
    DEFER(delete op_test);
    op_test->call();
    // EXPECT_EQ(200, op_test->status_code);
    logi("status_code: {}", op_test->status_code);
    std::string buf;
    buf.resize(std_data_size);
    memset((void *)buf.data(), '\0', std_data_size);
    auto ret = op_test->resp.read((void *)buf.data(), std_data_size);

    logi("res: {}", buf);
}

SEQ_FUNC void test_1() {
    // static char socket_buf[] = "this is a http_client request body text for
    // socket stream";

    // static const char target[] = "https://reqres.in/api/users?page=1";
    // auto client = new_http_client();
    // DEFER(delete client);
    // auto op2 = client->new_operation(Verb::GET, target);
    // DEFER(delete op2);
    // op2->req.headers.content_length(0);
    // int ret = client->call(op2);
    // // GTEST_ASSERT_EQ(0, ret);
    // logi("ret={}", ret);

    // char resp_body_buf[1024*1000];
    // // EXPECT_EQ(sizeof(socket_buf), op2->resp.resource_size());
    // ret = op2->resp.read(resp_body_buf, sizeof(socket_buf));
    // // EXPECT_EQ(sizeof(socket_buf), ret);
    // resp_body_buf[sizeof(socket_buf) - 1] = '\0';
    // LOG_DEBUG(resp_body_buf);
    // logi("resp_body_buf: {}", resp_body_buf);
    // EXPECT_EQ(0, strcmp(resp_body_buf, socket_buf));
}

SEQ_FUNC void test_photon_http(const char *url, size_t url_len) {
    // auto client = photon::net::http::new_http_client(nullptr);
    // client->native_connect("www.baidu.com", 443);
    // // photon::net::http::http_or_s(bool cond)
    // client->call(Operation *op)
    // const char target[] = "https://www.baidu.com/";

    std::string url_(url, url_len);

    logi("url={}", url);

    // const char target[] = "https://reqres.in/api/users?page=1";
    // const char target[] = "https://www.baidu.com/";
    const char target[] = "https://api.bybit.com/v3/public/time";

    auto client = new_http_client();

    // client->set_proxy("http://192.168.2.100:1080");

    printf("100\n");
    auto op = client->new_operation(Verb::GET, target);
    printf("101\n");
    client->call(op);
    printf("102\n");

    auto body_size = op->resp.resource_size();
    printf("103\n");
    logi("body_size: {}", body_size);

    // if (op->resp.headers.content_length() != FLAGS_body_size) {
    //     LOG_ERROR(VALUE(op->resp.headers.content_length()), VALUE(errno),
    //     VALUE(i), VALUE(FLAGS_body_size)); res->failed = true;
    // }
    printf("104\n");

    std::string body_buf;
    body_buf.resize(body_size);
    printf("105\n");
    auto ret = op->resp.read((void *)body_buf.data(), body_size);
    printf("106\n");
    if (ret != body_size) {
        printf("107\n");
        loge("fail");
        return;
    }
    printf("108\n");

    logi("body_buf 1");
    logi("body_buf: {}", body_buf);
    logi("body_buf 2");

    // auto data_size = op->resp.headers.content_length();

    // logi("status_code: {}", op->status_code);
    // std::string buf;
    // buf.resize(data_size);
    // memset((void *)buf.data(), '\0', data_size);
    // auto ret = op->resp.read((void *)buf.data(), data_size);

    // logi("res: {}", buf);

    delete op;
    delete client;

    // EXPECT_EQ(std_data_size, ret);
    // EXPECT_TRUE(buf == std_data);
    // for (int i = 0; i < buf.size(); i++) {
    //     if (buf[i] != std_data[i]) {
    //         LOG_ERROR("first occurrence of difference at: ", i);
    //         break;
    //     }
    // }
}

class StringStream {
    std::string s;

  public:
    std::string &str() { return s; }

    size_t write(void *c, size_t n) {
        // LOG_DEBUG("CALL WRITE");
        s.append((char *)c, n);
        return n;
    }
};

void test_curl() {
    for (int i = 0; i < 10; i++) {
        clock_t start, end;
        start = clock();
        StringStream buffer;
        // client->set_redirect(10).set_verbose(true);
        // client.append_header("const std::string &key", const std::string
        // &val)
        photon::net::cURL client;
        client.GET("https://api.bybit.com/v3/public/time", &buffer);
        end = clock();
        logi("{} ms, {}", double(end - start) / CLOCKS_PER_SEC * 1000.0,
             buffer.str().c_str());
        client.reset();
    }
    // buffer.reset();
    // client.reset();
}

void test_httpclient() {
    ssl::context ctx(ssl::context::tlsv13_client);
    ctx.set_default_verify_paths();
    std::string host = "api.bybit.com";
    std::string port = "443";
    httpx::HttpClient client(ctx, host, port);
    for (int i = 0; i < 10; i++) {
        clock_t start, end;
        start = clock();
        // httpx::Request request;
        std::string path = "/v3/public/time";
        beast::http::request<beast::http::string_body> request{HttpVerb::get,
                                                               path, 11};

        request.set(beast::http::field::host, host);
        request.set(beast::http::field::user_agent, "xt/1.0.1");
        auto res = client.performRequest(request);
        end = clock();
        logi("{} ms, {}", double(end - start) / CLOCKS_PER_SEC * 1000.0,
             res.body().c_str());
    }
}