#include "liblog.hpp"
#include "libthread.hpp"
#include "spdlog/common.h"
#include <absl/strings/match.h>
#include <photon/common/alog.h>
#include <photon/photon.h>
#include <photon/thread/thread-pool.h>
#include <photon/thread/thread.h>
#include <photon/thread/thread11.h>
#include <photon/thread/workerpool.h>
#include <fmtlog/fmtlog.h>
#include <spdlog/spdlog.h>

void *coro_log_run(void *arg) {
    for (;;) {
        fmtlog::poll();
        photon::thread_usleep(100);
        // photon::thread_yield();
    }
    return nullptr;
}

static int64_t log_mode = 0; // 0-fmtlog 1-spdlog

void logFullCB(void *) {
    // fmtlog::poll(true);
}

SEQ_FUNC void seq_init_log(uint8_t level, int64_t mode) {
    log_mode = mode;

    // DBG = 0,
    // INF,
    // WRN,
    // ERR,
    // OFF

    if (mode == 0) {
        fmtlog::setLogLevel(static_cast<fmtlog::LogLevel>(level));

        auto work_pool = seq_photon_work_pool();

        work_pool->thread_migrate(photon::thread_create(coro_log_run, nullptr),
                                  -1UL);
    } else if (mode == 1) {
        switch (level) {
        case 0:
            spdlog::set_level(spdlog::level::debug);
            break;
        case 1:
            spdlog::set_level(spdlog::level::info);
            break;
        case 2:
            spdlog::set_level(spdlog::level::warn);
            break;
        case 3:
            spdlog::set_level(spdlog::level::err);
            break;
        case 4:
            spdlog::set_level(spdlog::level::off);
            break;
        default:
            spdlog::set_level(spdlog::level::off);
            break;
        }
    }
}

SEQ_FUNC void seq_logvd(const char *s, size_t sLen) {
    std::string str(s, sLen);
    if (log_mode == 0) {
        logd("{}", str);
    } else if (log_mode == 1) {
        spdlog::debug(str.c_str());
    }
}

SEQ_FUNC void seq_logvi(const char *s, size_t sLen) {
    std::string str(s, sLen);
    if (log_mode == 0) {
        logi("{}", str);
    } else if (log_mode == 1) {
        spdlog::info(str.c_str());
    }
}

SEQ_FUNC void seq_logvw(const char *s, size_t sLen) {
    std::string str(s, sLen);
    if (log_mode == 0) {
        logw("{}", str);
    } else if (log_mode == 1) {
        spdlog::warn(str.c_str());
    }
}

SEQ_FUNC void seq_logve(const char *s, size_t sLen) {
    std::string str(s, sLen);
    if (log_mode == 0) {
        loge("{}", str);
    } else if (log_mode == 1) {
        spdlog::error(str);
    }
}

SEQ_FUNC void seq_log_test() {
    fmtlog::setLogLevel(fmtlog::LogLevel::INF);

    logi("abc33223");

    // set global default logger output to null
    set_log_output(log_output_null);
    // set_log_output(log_output_stdout);
    set_log_output_level(ALOG_INFO);

    logi("a debug");
    logi("a info");

    auto j = 100;
    auto count = 1000;
    auto durationMs = 1000000;
    logi("mutex {}: total acquisitions: {}, total wait time: {}ms", j, count,
         durationMs);
    std::string s = "abc";
    logi("a: {}", s);
    logi("abc");

    std::string_view sv = "hello";
}
