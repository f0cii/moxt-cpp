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

SEQ_FUNC std::atomic<bool> *seq_atomic_bool_new(bool b) {
    return new std::atomic<bool>(b);
}

SEQ_FUNC void seq_atomic_bool_free(std::atomic<bool> *p) { delete p; }

SEQ_FUNC bool seq_atomic_bool_load(std::atomic<bool> *p) { return p->load(); }

SEQ_FUNC void seq_atomic_bool_store(std::atomic<bool> *p, bool v) {
    p->store(v);
}

SEQ_FUNC std::atomic<int64_t> *seq_atomic_int64_new(int64_t i) {
    return new std::atomic<int64_t>(i);
}

SEQ_FUNC void seq_atomic_int64_free(std::atomic<int64_t> *p) { delete p; }

SEQ_FUNC int64_t seq_atomic_int64_load(std::atomic<int64_t> *p) {
    return p->load();
}

SEQ_FUNC void seq_atomic_int64_store(std::atomic<int64_t> *p, int64_t i) {
    p->store(i);
}