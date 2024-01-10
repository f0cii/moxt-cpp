#include "libthread.hpp"
#include "common.hpp"
#include "moxt/cclient.hpp"
#include "photon/photon.h"
#include "photon/thread/thread.h"
#include <chrono>
#include <cmath>
#include <csignal>
#include <photon/common/alog.h>
#include <photon/io/signal.h>
#include <photon/net/http/client.h>
#include <photon/net/socket.h>
#include <photon/thread/thread-pool.h>
#include <photon/thread/stack-allocator.h>

static photon::WorkPool *work_pool = nullptr;

SEQ_FUNC photon::WorkPool *seq_photon_work_pool() { return work_pool; }

SEQ_FUNC void seq_init_photon_work_pool(size_t vcpu_num) {
    // work_pool = new photon::WorkPool(pool_size, photon::INIT_EVENT_EPOLL,
    // 64);
    work_pool = new photon::WorkPool(vcpu_num, photon::INIT_EVENT_DEFAULT,
                                     photon::INIT_IO_DEFAULT);
}

SEQ_FUNC void
seq_photon_thread_create_and_migrate_to_work_pool(task_entry entry, void *arg) {
    // -1 it will choose the next vCPU in pool (round-robin).
    // work_pool->thread_migrate(photon::thread_create(entry, arg), -1UL);
    work_pool->thread_migrate(photon::thread_create(entry, arg));
}

SEQ_FUNC void seq_photon_thread_migrate_to_work_pool(photon::thread *th) {
    // -1 it will choose the next vCPU in pool (round-robin).
    work_pool->thread_migrate(th);
}

// typedef void *(*task_entry)(void *arg);
// typedef void (*task_callback)(void *result);

void *task_sync(void *arg) {
    printf("task_sync\n");
    return nullptr;
}

void free_far(FuncArgResult *far) {
    if (far != nullptr) {
        far->free();
        delete far;
        far = nullptr;
    }
}

SEQ_FUNC void seq_photon_set_log_output(uint8_t mode) {
    // set_log_output_level(ALOG_INFO);
    if (mode == 0) {
        set_log_output(log_output_stdout);
    } else {
        set_log_output(log_output_null);
    }
}

SEQ_FUNC int seq_photon_init_default() {
    // photon::set_photon_thread_stack_allocator();
    photon::use_pooled_stack_allocator();
    // photon::set_bypass_threadpool();
    int ret = photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_DEFAULT);
    if (ret != 0) {
        return ret;
    }

    // curl_global_init(CURL_GLOBAL_DEFAULT);
    init_curl_pool();

    return 0;
}

SEQ_FUNC int seq_photon_init() {
    int ret = photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_DEFAULT);
    // int ret = photon::init(photon::INIT_EVENT_DEFAULT,
    // photon::INIT_IO_LIBAIO); if (ret < 0) {
    //     LOG_ERROR_RETURN(0, -1, "failed to init photon environment");
    // }
    return ret;
}

SEQ_FUNC void seq_photon_fini() { photon::fini(); }

SEQ_FUNC photon::WorkPool *seq_photon_workpool_new(size_t pool_size) {
    // auto pool = new photon::WorkPool(pool_size, photon::INIT_EVENT_EPOLL,
    // 64);
    auto pool = new photon::WorkPool(pool_size, photon::INIT_EVENT_DEFAULT,
                                     photon::INIT_IO_DEFAULT);
    return pool;
}

SEQ_FUNC void seq_photon_workpool_free(photon::WorkPool *pool) {
    delete pool;
    pool = nullptr;
}

SEQ_FUNC void
seq_photon_join_current_vcpu_into_workpool(photon::WorkPool *pool) {
    pool->join_current_vcpu_into_workpool();
}

SEQ_FUNC int seq_photon_workpool_get_vcpu_num(photon::WorkPool *pool) {
    return pool->get_vcpu_num();
}

SEQ_FUNC void seq_photon_workpool_async_call(photon::WorkPool *pool,
                                             task_entry entry, void *arg) {
    pool->async_call(new auto([entry, arg] {
        auto arg_ = (FuncArgResult *)arg;
        auto result = (FuncArgResult *)entry(arg);
        free_far(arg_);
        free_far(result);
    }));
}

SEQ_FUNC void seq_photon_workpool_async_call_with_cb(photon::WorkPool *pool,
                                                     task_entry entry,
                                                     void *arg,
                                                     task_callback cb) {
    pool->async_call(new auto([entry, arg, cb] {
        auto arg_ = (FuncArgResult *)arg;
        auto result = entry(arg);
        if (cb != nullptr) {
            cb(result);
        }
        free_far(arg_);
    }));
}

SEQ_FUNC void seq_photon_workpool_call(photon::WorkPool *pool, task_entry entry,
                                       void *arg) {
    pool->call([entry, arg] {
        auto arg_ = (FuncArgResult *)arg;
        auto result = (FuncArgResult *)entry(arg);
        free_far(arg_);
        free_far(result);
    });
}

SEQ_FUNC void seq_photon_workpool_call_with_cb(photon::WorkPool *pool,
                                               task_entry entry, void *arg,
                                               task_callback cb) {
    pool->call([entry, arg, cb] {
        auto arg_ = (FuncArgResult *)arg;
        auto result = (FuncArgResult *)entry(arg);
        if (cb != nullptr) {
            cb(result);
        }
        free_far(arg_);
        free_far(result);
    });
}

SEQ_FUNC photon::ThreadPoolBase *seq_photon_thread_pool_new(uint32_t capacity) {
    return photon::ThreadPoolBase::new_thread_pool(capacity);
}

SEQ_FUNC void seq_photon_thread_pool_free(photon::ThreadPoolBase *pool) {
    photon::delete_thread_pool(pool);
    pool = nullptr;
}

// void *thread_entry(void *arg) {
//     printf("thread_entry\n");
//     return nullptr;
// }

SEQ_FUNC photon::thread *
seq_photon_thread_pool_thread_create(photon::ThreadPoolBase *pool,
                                     photon::thread_entry start, void *arg) {
    auto th = pool->thread_create(start, arg);
    return th;
}

SEQ_FUNC photon::TPControl *
seq_photon_thread_pool_thread_create_ex(photon::ThreadPoolBase *pool,
                                        photon::thread_entry start, void *arg) {
    auto th = pool->thread_create_ex(start, arg);
    return th;
}

SEQ_FUNC void seq_photon_tpcontrol_join(photon::ThreadPoolBase *pool,
                                        photon::TPControl *pCtrl) {
    pool->join(pCtrl);
}

SEQ_FUNC photon::thread *seq_photon_thread_create(photon::thread_entry start,
                                                  void *arg,
                                                  uint64_t stack_size,
                                                  uint16_t reserved_space) {
    // stack_size = photon::DEFAULT_STACK_SIZE
    // reserved_space = 0
    return photon::thread_create(start, arg, stack_size, reserved_space);
}

SEQ_FUNC photon::join_handle *seq_photon_thread_enable_join(photon::thread *th,
                                                            bool flag) {
    // flag = true
    return photon::thread_enable_join(th, flag);
}

SEQ_FUNC void seq_photon_thread_join(photon::join_handle *jh) {
    photon::thread_join(jh);
}

SEQ_FUNC void seq_photon_thread_yield() { photon::thread_yield(); }

SEQ_FUNC void seq_photon_thread_sleep_s(uint64_t seconds) {
    photon::thread_sleep(seconds);
}

SEQ_FUNC void seq_photon_thread_sleep_ms(uint64_t mseconds) {
    photon::thread_usleep(mseconds * 1000);
}

SEQ_FUNC void seq_photon_thread_sleep_us(uint64_t useconds) {
    photon::thread_usleep(useconds);
}

// void photon::thread_interrupt(photon::thread* th, int error_number = EINTR);
SEQ_FUNC void seq_photon_thread_interrupt(photon::thread *th,
                                          int error_number) {
    // error_number = EINTR
    photon::thread_interrupt(th, error_number);
}

// int photon::thread_shutdown(photon::thread* th, bool flag = true);
SEQ_FUNC int seq_photon_thread_shutdown(photon::thread *th, bool flag) {
    // flag = true
    return photon::thread_shutdown(th, flag);
}

// int photon::thread_migrate(photon::thread* th, photon::vcpu_base* vcpu);
SEQ_FUNC int seq_photon_thread_migrate(photon::thread *th,
                                       photon::vcpu_base *vcpu) {
    return photon::thread_migrate(th, vcpu);
}

// vcpu_base* get_vcpu(thread* th = CURRENT);
SEQ_FUNC photon::vcpu_base *seq_get_vcpu(photon::thread *th) {
    // th = photon::CURRENT
    return get_vcpu(th);
}

SEQ_FUNC photon::thread *seq_photon_current_thread() { return photon::CURRENT; }

SEQ_FUNC void seq_thread_test() {
    // init_log(Severity::DEBUG);

    printf("seq_thread_test start\n");
    photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_LIBAIO);
    DEFER(photon::fini());
    auto pool = new photon::WorkPool(4, photon::INIT_EVENT_EPOLL, 64);
    DEFER(delete pool);

    // pool->async_call(task_sync);

    auto start = std::chrono::steady_clock::now();
    pool->async_call(new auto([&, start] {
        // printf("call start\n");
        logi("call start");
        // logi("start={}", start.time_since_epoch().count());
        logi("call start 1");
        auto end = std::chrono::steady_clock::now();
        // sum.fetch_add(std::chrono::duration_cast<std::chrono::nanoseconds>(end
        // - start).count(), std::memory_order_relaxed); sem.signal(1);
    }));

    pool->async_call(new auto([&] {
        // printf("call start\n");
        logi("call start");
        // logi("start={}", start.time_since_epoch().count());
        logi("call start 1");
        auto end = std::chrono::steady_clock::now();
        // sum.fetch_add(std::chrono::duration_cast<std::chrono::nanoseconds>(end
        // - start).count(), std::memory_order_relaxed); sem.signal(1);
    }));

    logi("seq_thread_test 1");
    // printf("seq_thread_test 1\n");
    // while (true) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // }
    logi("seq_thread_test end\n");
}

void *thread_entry(void *arg) {
    printf("thread_entry\n");
    return nullptr;
}

SEQ_FUNC void seq_thread_test1() {
    // init_log(Severity::DEBUG);

    printf("seq_thread_test start\n");
    photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_LIBAIO);
    DEFER(photon::fini());

    auto pool = photon::ThreadPoolBase::new_thread_pool(2);

    auto th = pool->thread_create(thread_entry, nullptr);

    logi("seq_thread_test 0");

    photon::thread_usleep(EINTR);

    logi("seq_thread_test 1");

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logi("seq_thread_test end");

    photon::delete_thread_pool(pool);
}

SEQ_FUNC void seq_init_signal(signal_handle_t handle) {
    signal(SIGTERM, handle);
    signal(SIGINT, handle);
}

SEQ_FUNC void seq_init_photon_signal(signal_handle_t handle) {
    photon::sync_signal(SIGTERM, handle);
    photon::sync_signal(SIGINT, handle);
}

SEQ_FUNC TimedClosureExecutor *
seq_photon_timed_closure_executor_new(uint64_t default_timeout,
                                      timed_closure_entry entry,
                                      int64_t closurePtr, bool repeating) {
    TimedClosureExecutor *executor =
        new TimedClosureExecutor(default_timeout, entry, closurePtr, repeating);
    return executor;
}

SEQ_FUNC void
seq_photon_timed_closure_executor_free(TimedClosureExecutor *executor) {
    delete executor;
}

SEQ_FUNC photon::Timer *
seq_photon_timed_closure_executor_get_timer(TimedClosureExecutor *executor) {
    return executor->getTimer();
}

SEQ_FUNC photon::Timer *seq_photon_timer_new(uint64_t default_timeout,
                                             timer_entry entry,
                                             bool repeating) {
    auto timer = new photon::Timer(default_timeout, entry, repeating);
    return timer;
}

SEQ_FUNC void seq_photon_timer_free(photon::Timer *timer) { delete timer; }

SEQ_FUNC int seq_photon_timer_reset(photon::Timer *timer,
                                    uint64_t new_timeout) {
    return timer->reset(new_timeout);
}

SEQ_FUNC int seq_photon_timer_cancel(photon::Timer *timer) {
    return timer->cancel();
}

SEQ_FUNC int seq_photon_timer_stop(photon::Timer *timer) {
    return timer->stop();
}

SEQ_FUNC photon::mutex *seq_photon_mutex_new() { return new photon::mutex(); }

SEQ_FUNC void seq_photon_mutex_free(photon::mutex *mu) { delete mu; }

SEQ_FUNC int seq_photon_mutex_lock(photon::mutex *mu, uint64_t timeout) {
    return mu->lock(timeout);
}

SEQ_FUNC int seq_photon_mutex_try_lock(photon::mutex *mu) {
    return mu->try_lock();
}

SEQ_FUNC void seq_photon_mutex_unlock(photon::mutex *mu) { mu->unlock(); }

SEQ_FUNC photon::spinlock *seq_photon_spinlock_new() {
    return new photon::spinlock();
}

SEQ_FUNC void seq_photon_spinlock_free(photon::spinlock *lock) { delete lock; }

SEQ_FUNC int seq_photon_spinlock_lock(photon::spinlock *lock) {
    return lock->lock();
}

SEQ_FUNC int seq_photon_spinlock_try_lock(photon::spinlock *lock) {
    return lock->try_lock();
}

SEQ_FUNC void seq_photon_spinlock_unlock(photon::spinlock *lock) {
    lock->unlock();
}

SEQ_FUNC photon::condition_variable *seq_photon_condition_variable_new() {
    return new photon::condition_variable();
}

SEQ_FUNC void
seq_photon_condition_variable_free(photon::condition_variable *cond) {
    delete cond;
}

SEQ_FUNC int
seq_photon_condition_variable_wait_no_lock(photon::condition_variable *cond,
                                           uint64_t timeout) {
    return cond->wait_no_lock(timeout);
}

SEQ_FUNC photon::thread *
seq_photon_condition_variable_notify_one(photon::condition_variable *cond) {
    return cond->notify_one();
}

SEQ_FUNC int
seq_photon_condition_variable_notify_all(photon::condition_variable *cond) {
    return cond->notify_all();
}

SEQ_FUNC photon::semaphore *seq_photon_semaphore_new() {
    return new photon::semaphore();
}

SEQ_FUNC void seq_photon_semaphore_free(photon::semaphore *sem) { delete sem; }

SEQ_FUNC int seq_photon_semaphore_wait(photon::semaphore *sem, uint64_t count,
                                       uint64_t timeout) {
    return sem->wait(count, timeout);
}

SEQ_FUNC int seq_photon_semaphore_signal(photon::semaphore *sem,
                                         uint64_t count) {
    return sem->signal(count);
}

SEQ_FUNC uint64_t seq_photon_semaphore_count(photon::semaphore *sem) {
    return sem->count();
}

SEQ_FUNC photon::rwlock *seq_photon_rwlock_new() {
    return new photon::rwlock();
}

SEQ_FUNC void seq_photon_rwlock_free(photon::rwlock *rwlock) { delete rwlock; }

// mode: RLOCK / WLOCK
// constexpr int RLOCK=0x1000;
// constexpr int WLOCK=0x2000;
SEQ_FUNC int seq_photon_rwlock_lock(photon::rwlock *rwlock, int mode,
                                    uint64_t timeout) {
    return rwlock->lock(mode, timeout);
}

SEQ_FUNC int seq_photon_rwlock_unlock(photon::rwlock *rwlock) {
    return rwlock->unlock();
}
