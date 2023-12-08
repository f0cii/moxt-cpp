#ifndef TSC_TIME_HPP
#define TSC_TIME_HPP

#pragma once

#include <tscns.h>

extern TSCNS tscns;

void initTscTimer();

inline uint64_t nowTsc() { return tscns.rdns(); }

#endif // TSC_TIME_HPP