#include "libc.hpp"
#include "common.hpp"
#include <atomic>
#include <cstdio>
#include <signal.h>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

SEQ_FUNC int64_t seq_voidptr_to_int(void *p) {
    return reinterpret_cast<int64_t>(p);
}

SEQ_FUNC void *seq_int_to_voidptr(int64_t i) {
    return reinterpret_cast<void *>(i);
}

SEQ_FUNC std::atomic<bool> *seq_atomic_bool_new(bool b) {
    return new std::atomic<bool>(b);
}

SEQ_FUNC void seq_atomic_bool_free(std::atomic<bool> *p) { delete p; }

SEQ_FUNC bool seq_atomic_bool_load(std::atomic<bool> *p) { return p->load(); }

SEQ_FUNC void seq_atomic_bool_store(std::atomic<bool> *p, bool v) {
    p->store(v);
}

SEQ_FUNC std::atomic<int64_t> *seq_atomic_int64_new(int64_t i) {
    return new std::atomic<int64_t>(i);
}

SEQ_FUNC void seq_atomic_int64_free(std::atomic<int64_t> *p) { delete p; }

SEQ_FUNC int64_t seq_atomic_int64_load(std::atomic<int64_t> *p) {
    return p->load();
}

SEQ_FUNC void seq_atomic_int64_store(std::atomic<int64_t> *p, int64_t i) {
    p->store(i);
}

// Define a function pointer type for signal handlers
typedef void (*SignalHandler)(int);

// Exported function to register a custom signal handler
// Parameters:
//   signum: The signal number to catch
//   handler: The custom signal handler function to be called when the signal is
//   received
SEQ_FUNC void seq_register_signal_handler(int signum, SignalHandler handler) {
    struct sigaction sigAction;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;
    sigAction.sa_handler = handler;

    if (sigaction(signum, &sigAction, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}