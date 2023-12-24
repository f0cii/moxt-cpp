//
// Created by ken on 2023/7/24.
//

#ifndef MOXT_WATCHDOG_HPP
#define MOXT_WATCHDOG_HPP

#include "moxt/common.hpp"
#include <boost/asio.hpp>
#include <functional>

/*
在调用Enable()后，经过一定时间后会调用回调函数。
当回调发生时，WatchDog将被禁用， 如果需要，可以再次调用Enable()或Reset()。
*/
class WatchDog {
  public:
    WatchDog(boost::asio::io_context &ioc, std::function<void()> callback);
    void Enable(int timeout);
    void Disable();
    void Reset();

  private:
    int timeout_;
    boost::asio::deadline_timer timer_;
    std::function<void()> callback_;
};

#endif // MOXT_WATCHDOG_HPP
