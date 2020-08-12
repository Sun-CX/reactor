//
// Created by suncx on 2020/8/10.
//

#ifndef REACTOR_BLOCKINGQUEUE_H
#define REACTOR_BLOCKINGQUEUE_H

#include "Mutex.h"
#include "Condition.h"
#include <deque>

using std::deque;
using std::move;

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

    /**
     * @param x 右值引用，而非万能引用
     */
    void en_queue(T &&x) {
        MutexGuard guard(mutex);
        queue.push_back(move(x));
        cond.notify();
    }

    T de_queue() {
        MutexGuard guard(mutex);

        /**
         * 必须为循环，不能为 if
         * 防止虚假唤醒
         */
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

#endif //REACTOR_BLOCKINGQUEUE_H
