#include "skiplist.hpp"
#include "moxt/common.hpp"
#include <iostream>

using namespace std;

SEQ_FUNC int test_skiplistx() {
#ifdef __linux__
    // cout << "skiplist test now starts..." << endl;

    // //给一个最大的整型值
    // SkipList<int, int> skipList(0x7fffffff);

    // int length = 10;

    // for (int i = 1; i <= length; ++i) {
    //     skipList.insert(i, i + 200);
    // }

    // cout << "The number of elements in skiplist is:" << skipList.size() <<
    // endl;

    // if (skipList.size() != length) {
    //     cout << "insert failur." << endl;
    // } else {
    //     cout << "insert success." << endl;
    // }

    // //测试查找
    // int value = -1;
    // int key = 9;
    // Node<int, int> *searchResult = skipList.search(key);
    // if (searchResult != nullptr) {
    //     value = searchResult->getValue();
    //     cout << "search result for key " << key << ":" << value << endl;
    // } else {
    //     cout << "search failure for key " << key << endl;
    // }

    // //重置value
    // value = -1;

    // //测试移除,测试不通过
    // key = 6;
    // cout<<endl<<"start to remove"<<endl;
    // bool removeResult = skipList.remove(key, value);
    // if (removeResult) {
    //     cout << "removed node whose key is " << key << " and value is " <<
    //     value << endl;
    // } else {
    //     cout << "removed failure" << endl;
    // }

    // SkipList<int, int> skipList1(0x7fffffff, true);
    SkipList<int64_t, int64_t> skipList1(0x0, false);

    struct timespec start, end;
    int64_t key0 = 100;
    int64_t value0 = 1000;
    int N = 10000000;
    // int N = 10000000;

    /* Insert test */
    srandom(time(nullptr));
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < N; i++) {
        // int value = key[i] = (int)random();
        // skiplist_insert(list, key[i], value);
        skipList1.insert(key0, value0);
        skipList1.remove(key0, value0);

        // seq_skiplist_insert(list, key0, value0);
        // seq_skiplist_remove(list, key0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    // printf("time span: %ldms\n", (end.tv_sec - start.tv_sec)*1000 +
    // (end.tv_nsec - start.tv_nsec)/1000000);
    auto timeSpanNs = (end.tv_sec - start.tv_sec) * 1000 * 1000000 +
                      (end.tv_nsec - start.tv_nsec);
    printf("(insert+remove) time span: %ld ns N=%d\n", timeSpanNs, N);
    auto n = N;
    printf("(insert+remove) 耗时 %f ns\n", (float)timeSpanNs / (float)n);

    skipList1.insert(1, value0);
    skipList1.insert(5, value0);
    skipList1.insert(3, value0);
    skipList1.insert(4, value0);
    skipList1.insert(4, 1500);
    skipList1.insert(6, value0);
    skipList1.remove(5, value0);

    skipList1.dumpAllNodes();

    auto v = skipList1.search(3);

    printf("key: %ld value: %ld\n", v->getKey(), v->getValue());

    printf("OK\n");
#endif
    return 0;
}
