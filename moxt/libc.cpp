#include "libc.hpp"
#include "common.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

SEQ_FUNC int64_t seq_address_of(void *p) {
    return reinterpret_cast<int64_t>(p);
}
