#pragma once

#include "moxt/common.hpp"
#include <fast_float/fast_float.h>

inline int64_t strtoi(const std::string_view &s) {
    int64_t result = 0;
    for (char c : s) {
        result = result * 10 + (c - '0');
    }
    return result;
}

inline int64_t strtoi(const std::string &s) {
    int64_t result = 0;
    for (char c : s) {
        result = result * 10 + (c - '0');
    }
    return result;
}

inline double strtod(const std::string_view &str) {
    double value = 0;
    auto answer =
        fast_float::from_chars(str.data(), str.data() + str.size(),
                               value); // fast_float::chars_format::fixed
    return value;
}

inline double strtod(const std::string &str) {
    double value = 0;
    auto answer =
        fast_float::from_chars(str.data(), str.data() + str.size(), value);
    return value;
}
