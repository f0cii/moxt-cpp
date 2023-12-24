#ifndef MOXT_COMMON_HPP
#define MOXT_COMMON_HPP

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <snmalloc/override/libc.h>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/format.h>

// for windows, linux 平台不需要以下2行
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef _WIN32
#undef max
#undef min
#endif

// #include <fmtlog/fmtlog.h>
// #include <spdlog/spdlog.h>
#include <quill/Quill.h>

// #define logd spdlog::debug
// #define logi spdlog::info
// #define logw spdlog::warn
// #define loge spdlog::error

#define logd(fmt, ...) QUILL_LOG_DEBUG(quill::get_logger(), fmt, ##__VA_ARGS__)
#define logi(fmt, ...) QUILL_LOG_INFO(quill::get_logger(), fmt, ##__VA_ARGS__)
#define logw(fmt, ...) QUILL_LOG_WARNING(quill::get_logger(), fmt, ##__VA_ARGS__)
#define loge(fmt, ...) QUILL_LOG_ERROR(quill::get_logger(), fmt, ##__VA_ARGS__)

#define SEQ_FUNC extern "C"

#endif // MOXT_COMMON_HPP
