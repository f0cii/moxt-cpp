#include "libsample.hpp"
#include "common.hpp"

SEQ_FUNC int seq_add(int a, int b) { return a + b; }

SEQ_FUNC int seq_add_callback(int a, int b, SeqAddCallbackFunc callback) {
    return callback(a, b);
}

SEQ_FUNC int seq_add_with_exception0(int a, int b) {
    auto c = 1 / 0;
    return c;
}

SEQ_FUNC int seq_add_with_exception1(int a, int b) {
    throw std::runtime_error("error 1000");
}