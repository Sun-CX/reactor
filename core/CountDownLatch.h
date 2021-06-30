//
// Created by suncx on 2020/8/9.
//

#ifndef REACTOR_COUNTDOWNLATCH_H
#define REACTOR_COUNTDOWNLATCH_H

#include "Condition.h"

namespace reactor::core {
    class CountDownLatch final : public NonCopyable {
    private:
        mutable Mutex mutex;
        Condition cond;
        int count;
    public:
        explicit CountDownLatch(int count);

        void wait();

        void count_down();

        int get_count() const;
    };
}

#endif //REACTOR_COUNTDOWNLATCH_H
