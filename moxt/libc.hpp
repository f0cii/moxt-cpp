#ifndef MOXT_LIBC_HPP
#define MOXT_LIBC_HPP

#include "common.hpp"
#include <snmalloc/snmalloc.h>

using namespace snmalloc;

template <typename T, typename... Args> T *mem_new(Args... args) {
    void *p = ThreadAlloc::get().alloc(sizeof(T));
    return new (p) T(args...);
    // void *p = malloc(sizeof(T));
    // return new (p) T(args...);
}

template <typename T> T *mem_dup(T &&t) {
    void *p = ThreadAlloc::get().alloc(sizeof(T));
    return new (p) T(std::forward<T>(t));
    // void *p = malloc(sizeof(T));
    // return new (p) T(std::forward<T>(t));
}

template <typename T> void mem_delete(T *obj) {
    obj->~T();
    ThreadAlloc::get().dealloc(obj);
    // free(obj);
}

// #define ct_new new
// #define ct_dup new
// #define ct_delete delete

#endif