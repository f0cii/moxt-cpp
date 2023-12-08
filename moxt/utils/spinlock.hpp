#ifndef MOXT_UTILS_SPINLOCK_HPP
#define MOXT_UTILS_SPINLOCK_HPP

#pragma once

#include <cstdio>
#include <cstdlib>

#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>

#define CACHE_LINE_SIZE 64U

#if defined(_MSC_VER)
#define HARDWARE_PAUSE() _mm_pause();
#define _ENABLE_ATOMIC_ALIGNMENT_FIX 1 // MSVC atomic alignment fix.
#define ATOMIC_ALIGNMENT 4
#else
#define ATOMIC_ALIGNMENT 16
// #if defined(__clang__) || defined(__GNUC__)
// #define HARDWARE_PAUSE()            __builtin_ia32_pause();
// #endif
#if defined(__x86_64__)
#define HARDWARE_PAUSE() __builtin_ia32_pause()
#elif defined(__aarch64__)
#define HARDWARE_PAUSE() std::this_thread::yield()
#endif
#endif

#define should_yield_not_pause false

/**
 * Simple, efficient spin-lock implementation.
 * A function that takes a void lambda function can be used to
 * conveiniently do something which will be protected by the lock.
 * @cite Credit to Erik Rigtorp https://rigtorp.se/spinlock/
 */
class CSpinLock {
    std::atomic<bool> lock_flag;

  public:
    CSpinLock() : lock_flag{false} {}

    void do_work_through_lock(const std::function<void()> functor) {
        lock();
        functor();
        unlock();
    }

    void lock() {
        while (true) {
            if (!lock_flag.exchange(true, std::memory_order_acquire)) {
                break;
            }

            while (lock_flag.load(std::memory_order_relaxed)) {
                should_yield_not_pause ? std::this_thread::yield()
                                       : HARDWARE_PAUSE();
            }
        }
    }

    void unlock() { lock_flag.store(false, std::memory_order_release); }
};

#endif // MOXT_UTILS_SPINLOCK_HPP