//
// Created by ken on 2023/7/24.
//

#ifndef MOXT_ASIO_IOC_HPP
#define MOXT_ASIO_IOC_HPP

#include "common.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>

#include <memory>
#include <thread>
#include <vector>

// 线程池
class AsioIOC {
  public:
    // 获取默认的线程池
    static AsioIOC &default_pool();

    // 获取线程池的io_context
    boost::asio::io_context &getIOContext() { return io_context_; }

    void run();

    void poll();

    void poll_one();

    // 停止线程池
    void stop();

  private:
    AsioIOC();

    AsioIOC(const AsioIOC &) = delete;

    AsioIOC &operator=(const AsioIOC &) = delete;

  private:
    static std::size_t default_threads_;
    boost::asio::io_context io_context_;
    std::unique_ptr<boost::asio::io_context::work> work_;
};

#endif // MOXT_ASIO_IOC_HPP
