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

SEQ_FUNC int64_t seq_address_of(void *p);

// #define ct_new new
// #define ct_dup new
// #define ct_delete delete

#endif