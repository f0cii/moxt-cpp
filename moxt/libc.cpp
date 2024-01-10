#include "libc.hpp"
#include "common.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

SEQ_FUNC int64_t seq_voidptr_to_int(void *p) {
    return reinterpret_cast<int64_t>(p);
}

SEQ_FUNC void *seq_int_to_voidptr(int64_t i) {
    return reinterpret_cast<void *>(i);
}
