#include "libthread.hpp"
#include <chrono>
#include <cmath>
#include <photon/common/alog.h>
#include <photon/net/http/client.h>
#include <photon/net/socket.h>
#include <photon/thread/thread-pool.h>

static photon::WorkPool *work_pool = nullptr;

SEQ_FUNC photon::WorkPool *seq_photon_work_pool() { return work_pool; }

SEQ_FUNC void seq_init_photon_work_pool(size_t vcpu_num) {
    // work_pool = new photon::WorkPool(pool_size, photon::INIT_EVENT_EPOLL,
    // 64);
    work_pool = new photon::WorkPool(vcpu_num, photon::INIT_EVENT_DEFAULT,
                                     photon::INIT_IO_NONE);
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

SEQ_FUNC int seq_photon_init_default() {
    // set_log_output(log_output_stdout);
    // set_log_output_level(ALOG_INFO);
    set_log_output(log_output_null);

    // int ret = photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_NONE);
    int ret = photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_LIBCURL);
    // int ret = photon::init(photon::INIT_EVENT_DEFAULT,
    // photon::INIT_IO_LIBAIO);
    if (ret != 0) {
        return ret;
    }

    // #ifdef __linux__
    //     ret = photon::net::et_poller_init();
    //     if (ret < 0)
    //         return -1;
    //     // DEFER(net::et_poller_fini());
    // #endif

    return 0;
}

SEQ_FUNC int seq_photon_init() {
    // int ret = photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_NONE);
    int ret = photon::init(photon::INIT_EVENT_DEFAULT, photon::INIT_IO_LIBAIO);
    // if (ret < 0) {
    //     LOG_ERROR_RETURN(0, -1, "failed to init photon environment");
    // }
    return ret;
}

SEQ_FUNC void seq_photon_fini() { photon::fini(); }

SEQ_FUNC photon::WorkPool *seq_photon_workpool_new(size_t pool_size) {
    auto pool = new photon::WorkPool(pool_size, photon::INIT_EVENT_EPOLL, 64);
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