#ifndef MOXT_LIBC_HPP
#define MOXT_LIBC_HPP

#include "common.hpp"
#include <snmalloc/snmalloc.h>

using namespace snmalloc;

// #define USE_SNMALLOC 1

template <typename T, typename... Args> T *mem_new(Args... args) {
#if defined(USE_SNMALLOC)
    void *p = ThreadAlloc::get().alloc(sizeof(T));
#else
    void *p = malloc(sizeof(T));
#endif
    return new (p) T(args...);
}

template <typename T> T *mem_dup(T &&t) {
#if defined(USE_SNMALLOC)
    void *p = ThreadAlloc::get().alloc(sizeof(T));
#else
    void *p = malloc(sizeof(T));
#endif
    return new (p) T(std::forward<T>(t));
}

template <typename T> void mem_delete(T *obj) {
#if defined(USE_SNMALLOC)
    if (obj != nullptr) {
        obj->~T();
        ThreadAlloc::get().dealloc(obj);
        obj = nullptr;
    }
#else
    if (obj != nullptr) {
        obj->~T();
        free(obj);
        obj = nullptr;
    }
#endif
}

SEQ_FUNC int64_t seq_voidptr_to_int(void *p);
SEQ_FUNC void *seq_int_to_voidptr(int64_t i);

SEQ_FUNC std::atomic<bool> *seq_atomic_bool_new(bool b);
SEQ_FUNC void seq_atomic_bool_free(std::atomic<bool> *p);
SEQ_FUNC bool seq_atomic_bool_load(std::atomic<bool> *p);
SEQ_FUNC void seq_atomic_bool_store(std::atomic<bool> *p, bool v);

SEQ_FUNC std::atomic<int64_t> *seq_atomic_int64_new(int64_t i);
SEQ_FUNC void seq_atomic_int64_free(std::atomic<int64_t> *p);
SEQ_FUNC int64_t seq_atomic_int64_load(std::atomic<int64_t> *p);
SEQ_FUNC void seq_atomic_int64_store(std::atomic<int64_t> *p, int64_t i);

#endif