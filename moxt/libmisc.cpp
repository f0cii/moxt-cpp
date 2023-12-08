#include "libmisc.hpp"
#include "libc.hpp"
#include "photon/common/callback.h"
#include "simdjson.h"
#include <exception>
#include <nanobench.h>
#include <photon/common/callback.h>
#include <photon/common/identity-pool.h>
#include <tuple>

static int _constructor(int **ptr) {
    // printf("_constructor 10\n");
    *ptr = new int(100);
    // printf("_constructor 11\n");
    return 0;
}

static int _destructor(int *ptr) {
    delete ptr;
    return 0;
}

static int asdf = 323;

IdentityPool<int, 100> *new_pool() {
    // Callback<int **> _ctor1(_constructor);
    // Callback<int *> _dtor1(_destructor);

    // int asdf = 323;

    // auto _ctor = [&](int **ptr) -> int {
    //     *ptr = new int(asdf++);
    //     return 0;
    // };
    // auto _dtor = [&](int *ptr) -> int {
    //     delete ptr;
    //     return 0;
    // };

    auto _ctor = [](int **ptr) -> int {
        *ptr = new int(asdf++);
        return 0;
    };
    auto _dtor = [](int *ptr) -> int {
        delete ptr;
        return 0;
    };

    auto *pool = new IdentityPool<int, 100>(_ctor, _dtor);
    pool->enable_autoscale();
    // auto *pool = new IdentityPool<int, 100>(_constructor, _destructor);
    return pool;
}

// ondemand::parser

typedef simdjson::ondemand::parser ondemand_parser;

static int _ondemand_parser_constructor(ondemand_parser **ptr) {
    // printf("_constructor 10\n");
    *ptr = new ondemand_parser(1024 * 640);
    // printf("_constructor 11\n");
    return 0;
}

static int _ondemand_parser_destructor(ondemand_parser *ptr) {
    delete ptr;
    return 0;
}

IdentityPool<ondemand_parser, 100> *new_ondemand_parser_pool() {
    auto _ctor = [](ondemand_parser **ptr) -> int {
        *ptr = new ondemand_parser(1024 * 640);
        return 0;
    };
    auto _dtor = [](ondemand_parser *ptr) -> int {
        delete ptr;
        return 0;
    };

    auto *pool = new IdentityPool<ondemand_parser, 100>(_ctor, _dtor);
    pool->enable_autoscale();
    // auto *pool = new IdentityPool<int, 100>(_constructor, _destructor);
    return pool;
}

SEQ_FUNC void test_identity_pool() {
    auto *pool = new_pool();

    std::vector<int *> vec;
    for (int i = 0; i < 10; i++) {
        auto x = pool->get();
        vec.emplace_back(x);
    }

    for (auto x : vec) {
        pool->put(x);
    }

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("pool", [&] {
        auto x = pool->get();
        pool->put(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("raw", [&] {
        int *x;
        _constructor(&x);
        _destructor(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    // delete_identity_pool(&pool);
}

SEQ_FUNC void test_ondemand_parser_pool() {
    auto *pool = new_ondemand_parser_pool();

    std::vector<ondemand_parser *> vec;
    for (int i = 0; i < 10; i++) {
        auto x = pool->get();
        vec.emplace_back(x);
    }

    for (auto x : vec) {
        pool->put(x);
    }

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("od_pool", [&] {
        auto x = pool->get();
        pool->put(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    ankerl::nanobench::Bench().minEpochIterations(10000000).run("od_raw", [&] {
        ondemand_parser *x;
        _ondemand_parser_constructor(&x);
        _ondemand_parser_destructor(x);
        ankerl::nanobench::doNotOptimizeAway(x);
    });

    // delete_identity_pool(&pool);
}