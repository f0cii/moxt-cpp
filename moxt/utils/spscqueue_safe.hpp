//
// Created by ken on 2023/7/17.
//

#ifndef MOXT_SPSCQUEUE_SAFE_HPP
#define MOXT_SPSCQUEUE_SAFE_HPP

#include "spinlock.hpp"
#include <rigtorp/SPSCQueue.h>

template <typename T, typename Allocator = std::allocator<T>>
class SPSCQueueSafe : private rigtorp::SPSCQueue<T, Allocator> {
  public:
    SPSCQueueSafe(const size_t capacity,
                  const Allocator &allocator = Allocator())
        : rigtorp::SPSCQueue<T, Allocator>(capacity, allocator) {}

    void push(const T &v) {
        lock_.lock();
        rigtorp::SPSCQueue<T, Allocator>::push(v);
        lock_.unlock();
    }

    bool pop(T &v) {
        bool ok = false;
        lock_.lock();
        auto ptr = rigtorp::SPSCQueue<T, Allocator>::front();
        if (ptr) {
            v = *ptr;
            rigtorp::SPSCQueue<T, Allocator>::pop();
            ok = true;
        }
        lock_.unlock();
        return ok;
    }

    size_t size() const {
        auto size = rigtorp::SPSCQueue<T, Allocator>::size();
        return size;
    }

  private:
    CSpinLock lock_;
};

#endif // MOXT_SPSCQUEUE_SAFE_HPP
