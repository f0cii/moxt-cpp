//
// Created by ken on 2023/7/24.
//

#include "asio_ioc.hpp"

AsioIOC &AsioIOC::default_pool() {
    static AsioIOC pool_;
    return pool_;
}

AsioIOC::AsioIOC() : work_(new boost::asio::io_context::work(io_context_)) {}

void AsioIOC::run() { io_context_.run(); }

void AsioIOC::poll() { io_context_.poll(); }

void AsioIOC::poll_one() { io_context_.poll_one(); }

void AsioIOC::stop() { io_context_.stop(); }