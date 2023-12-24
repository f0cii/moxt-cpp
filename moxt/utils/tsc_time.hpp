#ifndef TSC_TIME_HPP
#define TSC_TIME_HPP

#pragma once

#include <chrono>
#include <cstdint>
#include <tscns.h>

extern TSCNS tscns;

void initTscTimer();

inline uint64_t nowTsc() { return tscns.rdns(); }

inline uint64_t time_milliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

inline uint64_t time_microseconds() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}

inline uint64_t time_nanoseconds() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
        .count();
}
#endif // TSC_TIME_HPP