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

    void wait();

    void count_down();

    int get_count() const;
};

#endif //REACTOR_COUNTDOWNLATCH_H
