#include "snowflake.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::host_name;
using boost::asio::ip::tcp;

std::string getLocalIp() {
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host_name(), "");
    tcp::resolver::iterator iter = resolver.resolve(query);
    tcp::resolver::iterator end;

    std::string ip = "";

    while (iter != end) {
        tcp::endpoint endpoint = *iter++;
        if (endpoint.address().is_v4()) {
            if (ip.empty()) {
                ip = endpoint.address().to_string();
            }
            // printf("endpoint.address: %s\n",
            // endpoint.address().to_string().c_str());
        }
    }

    return ip;
}

// return workerId
uint32_t ipToWorkerId(const std::string &ipAddress) {
    uint32_t workerId = 0;
    std::string::const_iterator it = ipAddress.begin();
    size_t n = 0;
    while (it != ipAddress.end() && n < 3) {
        if (*it == '.') {
            ++it;
            ++n;
        } else {
            workerId = workerId * 10 + (*it - '0');
            ++it;
        }
    }
    // Ensure workerId is within range [0, 1023]
    return workerId % 1024;
}

bool Snowflake::initWorkerId() {
    const std::string &ip = getLocalIp();
    // logi("local ip: {}", ip);
    workerId_ = ipToWorkerId(ip);
    // logi("WorkerId init success: {}", workerId_);
    return true;
}

int64_t Snowflake::getId() {
    std::lock_guard<std::mutex> guard(mutex_);

    int64_t timestamp = getTimestamp();
    if (timestamp < lastTimestamp_) {
        loge("Clock back");
        return kFirstBitRevert & getIdByTs(timestamp);
    }

    return getIdByTs(timestamp);
}

// get snowflake id by timestamp
// update lastTimestamp_ or sequence_
int64_t Snowflake::getIdByTs(int64_t timestamp) {
    // if it is the same time, then the microsecond sequence
    if (lastTimestamp_ == timestamp) {
        sequence_ = (sequence_ + 1) & kMaxSequence;
        // if the microsecond sequence overflow
        if (sequence_ == 0) {
            // block to the next millisecond, get the new timestamp
            timestamp = nextTimestamp();
        }
    } else {
        sequence_ = 0;
    }
    lastTimestamp_ = timestamp;
    return (timestamp - kStartStmp) << kTimestampLeft |
           workerId_ << kWorkerIdLeft | sequence_;
}

int64_t Snowflake::getTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

// wait for the next millisecond
int64_t Snowflake::nextTimestamp() {
    int64_t timestamp = getTimestamp();
    while (timestamp <= lastTimestamp_) {
        timestamp = getTimestamp();
    }
    return timestamp;
}
