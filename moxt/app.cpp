//
// Created by ken on 2023/7/29.
//

#include "app.hpp"

#include "moxt/httpx/asio_ioc.hpp"
#include "moxt/utils/tsc_time.hpp"
#include <stdexcept>
#include <execinfo.h>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <csignal>

// This function is used for handle segmental fault
inline void segfaultHandler(int signal __attribute__((unused))) {
    void *stackArray[20];
    size_t size = backtrace(stackArray, 10);
    std::cerr << "Segmentation fault! backtrace: ";
    char **backtrace = backtrace_symbols(stackArray, size);
    for (size_t i = 0; i < size; i++) {
        std::cerr << "\t" << backtrace[i];
    }
    abort();
}

// This is terminate handle function
inline void exceptionHandler() {
    static bool triedThrow = false;
    try {
        if (!triedThrow) {
            triedThrow = true;
            throw;
        }
    } catch (const std::exception &e) {
        std::cerr << "Caught unhandled exception: " << e.what();
    } catch (...) {
    }
    void *stackArray[20];
    size_t size = backtrace(stackArray, 10);
    std::cerr << "Segmentation fault! backtrace: ";
    char **backtrace = backtrace_symbols(stackArray, size);
    for (size_t i = 0; i < size; i++) {
        std::cerr << "\t" << backtrace[i];
    }
    abort();
}

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

    std::set_terminate(exceptionHandler);
}