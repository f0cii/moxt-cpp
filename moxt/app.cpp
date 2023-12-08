//
// Created by ken on 2023/7/29.
//

#include "app.hpp"

#include "moxt/httpx/asio_ioc.hpp"
#include "moxt/utils/tsc_time.hpp"

void app_init() {
    // 设置本地化
#ifdef __linux
    setlocale(LC_ALL, "zh_CN.UTF-8");
#elif _WIN32
    setlocale(LC_ALL, "chs");
#elif __APPLE__
    setlocale(LC_ALL, "zh_CN.UTF-8");
#endif

    initTscTimer();

    AsioIOC::default_pool();
}