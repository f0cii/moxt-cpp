#include "liblog.hpp"
#include "libthread.hpp"
#include "spdlog/common.h"
// #include "spdlog/common.h"
#include <absl/strings/match.h>
#include <climits>
#include <photon/common/alog.h>
#include <photon/photon.h>
#include <photon/thread/thread-pool.h>
#include <photon/thread/thread.h>
#include <photon/thread/thread11.h>
#include <photon/thread/workerpool.h>
#include <quill/Quill.h>

#if defined(USE_FMTLOG)
void logcb(int64_t ns, fmtlog::LogLevel level, fmt::string_view location,
           size_t basePos, fmt::string_view threadName, fmt::string_view msg,
           size_t bodyPos, size_t logFilePos) {
    // Convert nanoseconds to hours, minutes, seconds, and milliseconds
    auto hours = ns / 3600000000000;
    auto minutes = (ns % 3600000000000) / 60000000000;
    auto seconds = (ns % 60000000000) / 1000000000;
    auto milliseconds = (ns % 1000000000) / 1000000;

    // Format the time
    auto timeStr = fmt::format("{:02}:{:02}:{:02}.{:03}", hours % 24, minutes,
                               seconds, milliseconds);

    // Map LogLevel to its corresponding string representation
    std::string logLevelStr;
    switch (level) {
    case fmtlog::LogLevel::DBG:
        logLevelStr = "DBG";
        break;
    case fmtlog::LogLevel::INF:
        logLevelStr = "INF";
        break;
    case fmtlog::LogLevel::WRN:
        logLevelStr = "WRN";
        break;
    case fmtlog::LogLevel::ERR:
        logLevelStr = "ERR";
        break;
    }

    fmt::print("## {}\n", msg);
}

void *coro_log_run(void *arg) {
    for (;;) {
        fmtlog::poll();
        photon::thread_usleep(100);
        // photon::thread_yield();
    }
    return nullptr;
}
#endif

SEQ_FUNC void seq_init_log(uint8_t level, const char *filename,
                           size_t filenameLen) {
    std::string filename_(filename, filenameLen);
    init_log(level, filename_);
}

void init_log(uint8_t level, const std::string &filename) {
#if defined(USE_FMTLOG)
    auto logLevel = static_cast<fmtlog::LogLevel>(level);
    fmtlog::setLogLevel(logLevel);
    if (!filename.empty()) {
        fmtlog::setLogFile(filename.c_str(), false);
    }
    // fmtlog::setLogCB(logcb, logLevel);
    auto work_pool = seq_photon_work_pool();
    work_pool->thread_migrate(photon::thread_create(coro_log_run, nullptr),
                              -1UL);
#elif defined(USE_SPDLOG)
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
    }

#else
    auto logLevel = static_cast<quill::LogLevel>(level);
    quill::get_logger()->set_log_level(logLevel); // quill::LogLevel::TraceL1

    std::shared_ptr<quill::Handler> console_handler = quill::stdout_handler();

    // ascii_time [thread_id] filename:line level_name logger_name - message
    std::string log_pattern = "%(ascii_time) %(filename):%(lineno) "
                              "%(level_name) - " // [%(process)] [%(thread)]
                              "%(message)";
    // std::string time_format = "%D %H:%M:%S.%Qms %z";
    std::string time_format = "%Y-%m-%d %H:%M:%S.%Qus %z";
    console_handler->set_pattern(log_pattern, time_format,
                                 quill::Timezone::LocalTime);

    quill::Config cfg;
    cfg.enable_console_colours = true;
    cfg.default_handlers.push_back(std::move(console_handler));

    if (!filename.empty()) {
        // Get or create a handler to the file - Write to a different file
        // auto filename = "mf.log";
        std::shared_ptr<quill::Handler> file_handler =
            quill::file_handler(filename, []() {
                quill::FileHandlerConfig cfg;
                cfg.set_open_mode('w');
                cfg.set_append_to_filename(
                    quill::FilenameAppend::StartDateTime);
                return cfg;
            }());
        file_handler->set_pattern(log_pattern, time_format,
                                  quill::Timezone::LocalTime);
        cfg.default_handlers.push_back(std::move(file_handler));
    }

    quill::configure(cfg);
    quill::start();
#endif
}

SEQ_FUNC void seq_set_log_file(const char *filename) {
    // if (log_mode == 0) {
    //     // fmtlog::setLogFile(filename, false);
    // } else if (log_mode == 1) {
    // }
}

SEQ_FUNC void seq_logvd(const char *s, size_t sLen) {
    std::string str(s, sLen);
    logd("{}", str);
}

SEQ_FUNC void seq_logvi(const char *s, size_t sLen) {
    std::string str(s, sLen);
    logi("{}", str);
}

SEQ_FUNC void seq_logvw(const char *s, size_t sLen) {
    std::string str(s, sLen);
    logw("{}", str);
}

SEQ_FUNC void seq_logve(const char *s, size_t sLen) {
    std::string str(s, sLen);
    loge("{}", str);
}

SEQ_FUNC void seq_log_test() {
    // fmtlog::setLogLevel(fmtlog::LogLevel::INF);

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
