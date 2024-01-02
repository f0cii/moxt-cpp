// https://github.com/HiWong/SkipListPro
//
// Created by Wang Allen on 2018/2/6.
//

#ifndef SKIPLISTPRO_SKIPLIST_H
#define SKIPLISTPRO_SKIPLIST_H

// 以下修改版本，修正了内存问题
// https://github.com/ioperations/SkipListPro

#include "Node.h"
#include "moxt/fixed12.hpp"
#include "random.h"
#include <cassert>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

// #define DEBUG

template <typename K, typename V> class SkipList {

  public:
    SkipList(K footerKey, bool isForward = true)
        : rnd(0x12345678), isForward(isForward) {
        createList(footerKey);
    }

    ~SkipList() { freeList(); }

    // 注意:这里要声明成Node<K,V>而不是Node,否则编译器会把它当成普通的类
    Node<K, V> *search(K key) const;

    bool insert(K key, V value, bool updateIfExists = false);

    bool remove(K key, V &value);

    int size() { return nodeCount; }

    int getLevel() { return level; }

    void dumpAllNodes();

    void topN(int n, std::vector<std::pair<K, V>> &v);

    void topN(int n, std::vector<OrderBookLevel> &v);

    Node<K, V> *begin() const {
        auto node = header->forward[0];
        if (node == footer) {
            return nullptr;
        }
        return node;
    }

    Node<K, V> *end() const { return nullptr; }

    Node<K, V> *next(Node<K, V> *node) const {
        if (node != nullptr && node->forward[0] != footer) {
            return node->forward[0];
        }
        return nullptr;
    }

    void dumpNodeDetail(Node<K, V> *node, int nodeLevel);

  private:
    // 初始化表
    void createList(K footerKey);

    // 释放表
    void freeList();

    // 创建一个新的结点，节点的层数为level
    void createNode(int level, Node<K, V> *&node);

    void createNode(int level, Node<K, V> *&node, K key, V value);

    // 随机生成一个level
    int getRandomLevel();

  private:
    bool isForward; // true-正向 false-反向
    int level;
    Node<K, V> *header;
    Node<K, V> *footer;

    size_t nodeCount;

    static const int MAX_LEVEL = 16;

    Random rnd;
};

template <typename K, typename V> void SkipList<K, V>::createList(K footerKey) {
    createNode(0, footer);

    footer->key = footerKey;
    this->level = 0;
    // 设置头结
    createNode(MAX_LEVEL, header);
    for (int i = 0; i < MAX_LEVEL; ++i) {
        header->forward[i] = footer;
    }
    nodeCount = 0;
}

template <typename K, typename V>
void SkipList<K, V>::createNode(int level, Node<K, V> *&node) {
    node = new Node<K, V>(0, 0);
    // 需要初始化数组
    // 注意:这里是level+1而不是level,因为数组是从0-level
    node->forward = new Node<K, V> *[level + 1];
    node->nodeLevel = level;
    assert(node != NULL);
};

template <typename K, typename V>
void SkipList<K, V>::createNode(int level, Node<K, V> *&node, K key, V value) {
    node = new Node<K, V>(key, value);
    // 需要初始化数组
    if (level > 0) {
        node->forward = new Node<K, V> *[level + 1];
    }
    node->nodeLevel = level;
    assert(node != NULL);
};

template <typename K, typename V> void SkipList<K, V>::freeList() {
    Node<K, V> *p = header;
    Node<K, V> *q;
    while (p != NULL && p != footer) {
        q = p->forward[0];
        delete p;
        p = q;
    }
    delete p;
}

template <typename K, typename V>
Node<K, V> *SkipList<K, V>::search(const K key) const {
    Node<K, V> *node = header;
    for (int i = level; i >= 0; --i) {
        // while ((node->forward[i])->key < key)
        while (isForward ? (node->forward[i])->key < key
                         : (node->forward[i])->key > key) {
            node = *(node->forward + i);
        }
    }
    node = node->forward[0];
    if (node->key == key) {
        return node;
    } else {
        return nullptr;
    }
};

template <typename K, typename V>
bool SkipList<K, V>::insert(K key, V value, bool updateIfExists) {
    Node<K, V> *update[MAX_LEVEL];

    Node<K, V> *node = header;

    for (int i = level; i >= 0; --i) {
        // while ((node->forward[i])->key < key)
        while (isForward ? (node->forward[i])->key < key
                         : (node->forward[i])->key > key) {
            node = node->forward[i];
        }
        update[i] = node;
    }
    // 首个结点插入时，node->forward[0]其实就是footer
    node = node->forward[0];

    // 如果key已存在，则直接返回false
    if (node->key == key) {
        if (updateIfExists) {
            node->value = value;
            return true;
        }
        return false;
    }

    int nodeLevel = getRandomLevel();

    if (nodeLevel > level) {
        nodeLevel = ++level;
        update[nodeLevel] = header;
    }

    // 创建新结点
    Node<K, V> *newNode;
    createNode(nodeLevel, newNode, key, value);

    // 调整forward指针
    for (int i = nodeLevel; i >= 0; --i) {
        node = update[i];
        newNode->forward[i] = node->forward[i];
        node->forward[i] = newNode;
    }
    ++nodeCount;

#ifdef DEBUG
    dumpAllNodes();
#endif

    return true;
};

template <typename K, typename V> void SkipList<K, V>::dumpAllNodes() {
    Node<K, V> *node = header;
    while (node->forward[0] != footer) {
        node = node->forward[0];
        // dumpNodeDetail(tmp, tmp->nodeLevel);
        if (node == nullptr) {
            continue;
        }
        // cout << "node->key:" << node->key << ",node->value:" <<
        // node->value
        // << endl;
        cout << "node-> key: " << Fixed12::newV(node->key).toString().c_str()
             << " value: " << Fixed12::newV(node->value).toString().c_str()
             << endl;
        // printf("key:%s value:%s\n", fixed_12_string_fp(node->key).c_str(),
        // fixed_12_string_fp(node->value).c_str()); cout <<
        // "----------------------------" << endl;
    }
    cout << endl;
}

template <typename K, typename V>
void SkipList<K, V>::topN(int n, std::vector<std::pair<K, V>> &v) {
    Node<K, V> *node = header;
    int count = 0;
    while (node->forward[0] != footer) {
        if (count >= n) {
            break;
        }
        node = node->forward[0];
        if (node == nullptr) {
            continue;
        }
        v.push_back(std::pair<K, V>(node->key, node->value));
        ++count;
    }
}

template <typename K, typename V>
void SkipList<K, V>::topN(int n, std::vector<OrderBookLevel> &v) {
    Node<K, V> *node = header;
    int count = 0;
    while (node->forward[0] != footer) {
        if (count >= n) {
            break;
        }
        node = node->forward[0];
        if (node == nullptr) {
            continue;
        }
        v.push_back(OrderBookLevel_t(Fixed12::newV(node->key),
                                     Fixed12::newV(node->value)));
        ++count;
    }
}

template <typename K, typename V>
void SkipList<K, V>::dumpNodeDetail(Node<K, V> *node, int nodeLevel) {
    if (node == nullptr) {
        return;
    }
    cout << "node->key:" << node->key << ",node->value:" << node->value << endl;
    // 注意是i<=nodeLevel而不是i<nodeLevel
    for (int i = 0; i <= nodeLevel; ++i) {
        // cout << "forward[" << i << "]:"
        //      << "key:" << node->forward[i]->key << ",value:" <<
        //      node->forward[i]->value
        //      << endl;
    }
}

template <typename K, typename V> bool SkipList<K, V>::remove(K key, V &value) {
    Node<K, V> *update[MAX_LEVEL];
    Node<K, V> *node = header;
    for (int i = level; i >= 0; --i) {
        // while ((node->forward[i])->key < key)
        while (isForward ? (node->forward[i])->key < key
                         : (node->forward[i])->key > key) {
            node = node->forward[i];
        }
        update[i] = node;
    }
    node = node->forward[0];
    // 如果结点不存在就返回false
    if (node->key != key) {
        return false;
    }

    value = node->value;
    for (int i = 0; i <= level; ++i) {
        if (update[i]->forward[i] != node) {
            break;
        }
        update[i]->forward[i] = node->forward[i];
    }

    // 释放结点
    delete node;

    // 更新level的值，因为有可能在移除一个结点之后，level值会发生变化，及时移除可避免造成空间浪费
    while (level > 0 && header->forward[level] == footer) {
        --level;
    }

    --nodeCount;

#ifdef DEBUG
    dumpAllNodes();
#endif

    return true;
};

template <typename K, typename V> int SkipList<K, V>::getRandomLevel() {
    int level = static_cast<int>(rnd.Uniform(MAX_LEVEL));
    if (level == 0) {
        level = 1;
    }
    return level;
}

// typedef SkipList<int64_t, int64_t> skiplist_t;
typedef SkipList<fixed12_t, fixed12_t> skiplist_t;

#endif // SKIPLISTPRO_SKIPLIST_H