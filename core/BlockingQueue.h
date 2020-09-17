//
// Created by suncx on 2020/8/10.
//

#ifndef REACTOR_BLOCKINGQUEUE_BENCH_TEST_H
#define REACTOR_BLOCKINGQUEUE_BENCH_TEST_H

#include "Condition.h"
#include <deque>

using std::deque;
using std::move;

/**
 * 线程安全的无界队列
 *
 * 注意：
 * 由于是无界队列，所以要避免生产者线程的生产速率大于消费者的消费速率这种情况，否则会出现产品堆积占用大量内存，造成系统运行不稳定
 *
 * 若想从根本上规避这个隐患，请使用有界阻塞队列 BoundedBlockingQueue
 */
template<class T>
class BlockingQueue {
private:
    mutable Mutex mutex;
    Condition cond;
    deque<T> queue;
public:
    BlockingQueue() : mutex(), cond(mutex), queue() {}

    void en_queue(const T &x) {
        MutexGuard guard(mutex);
        queue.push_back(x);
        cond.notify();
    }

    // 右值引用，非万能引用
    void en_queue(T &&x) {
        MutexGuard guard(mutex);
        queue.push_back(move(x));
        cond.notify();
    }

    T de_queue() {
        MutexGuard guard(mutex);
        // 此处必须为循环，不能为 if，防止虚假唤醒
        while (queue.empty()) cond.wait();
        T front(move(queue.front()));
        queue.pop_front();
        return front;
    }

    size_t size() const {
        MutexGuard guard(mutex);
        return queue.size();
    }
};

#endif //REACTOR_BLOCKINGQUEUE_BENCH_TEST_H
