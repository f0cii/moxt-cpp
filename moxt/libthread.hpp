#ifndef MOXT_LIBTHREAD_HPP
#define MOXT_LIBTHREAD_HPP

#include "common.hpp"
#include "libc.hpp"
#include <snmalloc/snmalloc.h>

#include <photon/photon.h>
#include <photon/thread/thread-pool.h>
#include <photon/thread/thread.h>
#include <photon/thread/thread11.h>
#include <photon/thread/timer.h>
#include <photon/thread/workerpool.h>

#include "libfar.hpp"

typedef void *(*task_entry)(void *arg);
typedef void (*task_callback)(void *result);
typedef uint64_t (*timer_entry)();
typedef uint64_t (*timed_closure_entry)(int64_t c);

class TimedClosureExecutor {
  public:
    TimedClosureExecutor(uint64_t default_timeout, timed_closure_entry entry,
                         int64_t closurePtr, bool repeating)
        : _onTimer(entry), _closurePtr(closurePtr) {
        _timer = new photon::Timer(
            default_timeout, {this, &TimedClosureExecutor::onTick}, repeating);
    }
    ~TimedClosureExecutor() { delete _timer; }

    uint64_t onTick() { return _onTimer(_closurePtr); }

    photon::Timer *getTimer() { return _timer; }

  private:
    photon::Timer *_timer;
    timed_closure_entry _onTimer;
    int64_t _closurePtr;
};

SEQ_FUNC void seq_init_photon_work_pool(size_t vcpu_num);

SEQ_FUNC photon::WorkPool *seq_photon_work_pool();

SEQ_FUNC void
seq_photon_thread_create_and_migrate_to_work_pool(task_entry entry, void *arg);

SEQ_FUNC void seq_photon_thread_migrate_to_work_pool(photon::thread *th);

SEQ_FUNC void seq_photon_set_log_output(uint8_t mode);

SEQ_FUNC int seq_photon_init_default();

SEQ_FUNC int seq_photon_init();

SEQ_FUNC void seq_photon_fini();

SEQ_FUNC photon::WorkPool *seq_photon_workpool_new(size_t pool_size);

SEQ_FUNC void seq_photon_workpool_free(photon::WorkPool *pool);

SEQ_FUNC void
seq_photon_join_current_vcpu_into_workpool(photon::WorkPool *pool);

SEQ_FUNC int seq_photon_workpool_get_vcpu_num(photon::WorkPool *pool);

SEQ_FUNC void seq_photon_workpool_async_call(photon::WorkPool *pool,
                                             task_entry entry, void *arg);

SEQ_FUNC void seq_photon_workpool_async_call_with_cb(photon::WorkPool *pool,
                                                     task_entry entry,
                                                     void *arg,
                                                     task_callback cb);

SEQ_FUNC void seq_photon_workpool_call(photon::WorkPool *pool, task_entry entry,
                                       void *arg);

SEQ_FUNC void seq_photon_workpool_call_with_cb(photon::WorkPool *pool,
                                               task_entry entry, void *arg,
                                               task_callback cb);

SEQ_FUNC photon::ThreadPoolBase *seq_photon_thread_pool_new(uint32_t capacity);

SEQ_FUNC void seq_photon_thread_pool_free(photon::ThreadPoolBase *pool);

SEQ_FUNC photon::thread *
seq_photon_thread_pool_thread_create(photon::ThreadPoolBase *pool,
                                     photon::thread_entry start, void *arg);

SEQ_FUNC photon::TPControl *
seq_photon_thread_pool_thread_create_ex(photon::ThreadPoolBase *pool,
                                        photon::thread_entry start, void *arg);

SEQ_FUNC void seq_photon_tpcontrol_join(photon::ThreadPoolBase *pool,
                                        photon::TPControl *pCtrl);

SEQ_FUNC photon::thread *seq_photon_thread_create(photon::thread_entry start,
                                                  void *arg,
                                                  uint64_t stack_size,
                                                  uint16_t reserved_space);

SEQ_FUNC photon::join_handle *seq_photon_thread_enable_join(photon::thread *th,
                                                            bool flag);

SEQ_FUNC void seq_photon_thread_join(photon::join_handle *jh);

SEQ_FUNC void seq_photon_thread_yield();

SEQ_FUNC void seq_photon_thread_sleep_s(uint64_t seconds);

SEQ_FUNC void seq_photon_thread_sleep_ms(uint64_t mseconds);

SEQ_FUNC void seq_photon_thread_sleep_us(uint64_t useconds);

SEQ_FUNC void seq_photon_thread_interrupt(photon::thread *th, int error_number);

SEQ_FUNC int seq_photon_thread_shutdown(photon::thread *th, bool flag);

SEQ_FUNC int seq_photon_thread_migrate(photon::thread *th,
                                       photon::vcpu_base *vcpu);

SEQ_FUNC photon::vcpu_base *seq_get_vcpu(photon::thread *th);

SEQ_FUNC photon::thread *seq_photon_current_thread();

SEQ_FUNC void seq_thread_test();

typedef void (*signal_handle_t)(int sig);

SEQ_FUNC void seq_init_signal(signal_handle_t handle);

SEQ_FUNC void seq_init_photon_signal(signal_handle_t handle);

SEQ_FUNC TimedClosureExecutor *
seq_photon_timed_closure_executor_new(uint64_t default_timeout,
                                      timed_closure_entry entry,
                                      int64_t closurePtr, bool repeating);

SEQ_FUNC void
seq_photon_timed_closure_executor_free(TimedClosureExecutor *executor);

SEQ_FUNC photon::Timer *
seq_photon_timed_closure_executor_get_timer(TimedClosureExecutor *executor);

SEQ_FUNC photon::Timer *seq_photon_timer_new(uint64_t default_timeout,
                                             timer_entry entry, bool repeating);

SEQ_FUNC int seq_photon_timer_reset(photon::Timer *timer, uint64_t new_timeout);

SEQ_FUNC int seq_photon_timer_cancel(photon::Timer *timer);

SEQ_FUNC int seq_photon_timer_stop(photon::Timer *timer);

#endif