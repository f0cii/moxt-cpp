
#ifndef SNOWFLAKE_HPP
#define SNOWFLAKE_HPP

#include "moxt/common.hpp"
#include <chrono>
#include <cstdint>
#include <tscns.h>

class Snowflake {
    friend int64_t getSequence(int64_t id);
    friend int64_t getWorkerId(int64_t id);
    friend int64_t getTimestamp(int64_t id);

  public:
    Snowflake() = default;

    static bool initWorkerId();

    int64_t getId();

  private:
    static int64_t getTimestamp();

    int64_t nextTimestamp();

    int64_t getIdByTs(int64_t timestamp);

    std::mutex mutex_;
    /*
     *  Snowflake id:
     *  timestampBit 38 bits (8.6 years) |
     *  workerBit 13 bits (8k workerid) |
     *  sequenceBit 12 bits (4 million per second) |
     */
    int64_t lastTimestamp_{-1};
    static inline int64_t workerId_{0};
    int64_t sequence_{0};

    static constexpr int64_t kStartStmp =
        1704038400000;                        // start time: 2024-01-01 00:00:00
    static constexpr int64_t kWorkerBit = 13; // worker id bit
    static constexpr int64_t kSequenceBit = 12; // sequence bit

    static constexpr int64_t kMaxWorkerId =
        (1 << kWorkerBit) - 1; // as worker id mask
    static constexpr int64_t kMaxSequence = (1 << kSequenceBit) - 1;

    static constexpr int64_t kWorkerIdLeft = kSequenceBit; // workerId left bits
    static constexpr int64_t kTimestampLeft = kSequenceBit + kWorkerBit;

    static constexpr int64_t kFirstBitRevert =
        0x9000000000000000; // revert the first bit
};

#endif // SNOWFLAKE_HPP