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

#include <fmtlog/fmtlog.h>

#define SEQ_FUNC extern "C"

#endif // MOXT_COMMON_HPP
