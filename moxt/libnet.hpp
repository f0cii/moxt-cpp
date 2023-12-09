#ifndef MOXT_LIBNET_HPP
#define MOXT_LIBNET_HPP

#include "common.hpp"

enum NetDriverType { STD_THREAD = 0, PHOTON = 1 };

class WebSocket;

typedef void (*OnConnectCallback)(WebSocket *ws);
typedef void (*OnHeartbeatCallback)(WebSocket *ws);
typedef void (*OnMessageCallback)(WebSocket *ws, const char *data, size_t len);

// 启动asio
SEQ_FUNC void seq_asio_run();
SEQ_FUNC void seq_asio_poll();
SEQ_FUNC void seq_asio_poll_one();

// 停止asio
SEQ_FUNC void seq_asio_stop();

SEQ_FUNC WebSocket *seq_websocket_new(const char *host, const char *port,
                                      const char *path, int tls_version);
SEQ_FUNC void seq_websocket_delete(WebSocket *p);
SEQ_FUNC void seq_websocket_connect(WebSocket *p);
SEQ_FUNC void seq_websocket_send(WebSocket *p, const char *text, size_t len);
SEQ_FUNC void seq_websocket_set_on_connect(WebSocket *p,
                                           OnConnectCallback callback);
SEQ_FUNC void seq_websocket_set_on_heartbeat(WebSocket *p,
                                             OnHeartbeatCallback callback);
SEQ_FUNC void seq_websocket_set_on_message(WebSocket *p,
                                           OnMessageCallback callback);

SEQ_FUNC void seq_init_net(int type);
SEQ_FUNC void test_1();
SEQ_FUNC void test_photon_http(const char *url, size_t url_len);

#endif