#include "libsample.hpp"

SEQ_FUNC int seq_add(int a, int b) { return a + b; }

SEQ_FUNC int seq_add_callback(int a, int b, SeqAddCallbackFunc callback) {
    return callback(a, b);
}