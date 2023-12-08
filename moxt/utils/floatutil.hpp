//
// Created by ken on 2023/7/11.
//

#ifndef MOXT_FLOATUTIL_HPP
#define MOXT_FLOATUTIL_HPP

#include <cstring>
#include <fast_float/fast_float.h>
#include <string>

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

#endif // MOXT_FLOATUTIL_HPP
