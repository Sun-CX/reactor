//
// Created by suncx on 2020/8/9.
//

#ifndef REACTOR_COUNTDOWNLATCH_H
#define REACTOR_COUNTDOWNLATCH_H

#include "NonCopyable.h"
#include "Mutex.h"
#include "Condition.h"

class CountDownLatch final : public NonCopyable {
private:
    mutable Mutex mutex;
    Condition cond;
    int count;              // 被 mutex 保护，所以无需使用 atomic_int

public:
    explicit CountDownLatch(int count);

    /**
     * 当计数器大于 0 时阻塞当前线程（将当前线程加入阻塞队列）
     */
    void wait();

    /**
     * 对计数器进行减 1 操作，当计数器减至 0 时，当前线程会唤醒阻塞队列里的所有线程
     */
    void count_down();

    int get_count() const;
};

#endif //REACTOR_COUNTDOWNLATCH_H
