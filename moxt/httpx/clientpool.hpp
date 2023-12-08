//
// Created by ken on 2023/7/13.
//

#ifndef MOXT_CONNECTION_POOL_HPP
#define MOXT_CONNECTION_POOL_HPP

#include "common.hpp"
#include "httpclient.hpp"

class HttpClientPool {
  public:
    using HttpClient = httpx::HttpClient;

    virtual ~HttpClientPool() = default;

    virtual HttpClient *acquire() = 0;

    virtual void release(HttpClient *client) = 0;

    static HttpClientPool *
    make(std::string host, std::string port,
         ssl::context::method method = ssl::context::tlsv13_client);
};

#endif // MOXT_CONNECTION_POOL_HPP
