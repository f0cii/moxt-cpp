//
// Created by ken on 2023/7/18.
//

#ifndef CPT_C_RATE_LIMITER_HPP
#define CPT_C_RATE_LIMITER_HPP

#include "moxt/common.hpp"
#include "moxt/utils/cringbuf.hpp"
#include "spinlock.hpp"
#include "tsc_time.hpp"

/// <summary>
/// 滑动窗口限频器
/// </summary>
class CRateLimiter {
  public:
    /// <summary>
    /// 最大请求数
    /// </summary>
    /// <remarks>
    /// 例如：每秒最多10个请求
    /// </remarks>
    CRateLimiter(int maxCount, uint64_t windowSize)
        : maxCount(maxCount), windowSize(windowSize),
          rb(static_cast<size_t>(maxCount)) {}

    /// <summary>
    /// 允许并记录请求
    /// </summary>
    inline bool allowAndRecordRequest() {
        auto now = nowTsc();

        auto windowStart = now - windowSize;

        spinLock.lock();

        // 更快遍历
        uint64_t f = 0;
        if (rb.size() > 0) {
            try {
                f = rb.front();
            } catch (std::exception &e) {
                f = 0;
            }
        }

        // logi("f: {}, windowStart: {}, rb.size(): {}", f, windowStart,
        // rb.size());
        if (f != 0 && windowStart <= f && rb.size() >= maxCount) {
            // logi("false");
            spinLock.unlock();
            return false;
        }

        rb.push_back(now);

        spinLock.unlock();

        // logi("now: {}, windowStart: {}, rb.size(): {}", now, windowStart,
        // rb.size());

        return true;
    }

  private:
    inline static bool inWindow(const uint64_t &t, const uint64_t &start,
                                const uint64_t &end) {
        return start <= t && t <= end;
    }

    int maxCount;
    uint64_t windowSize;
    baudvine::RingBuf<uint64_t> rb;
    CSpinLock spinLock;
};

#endif // CPT_C_RATE_LIMITER_HPP
