//
// Created by suncx on 2020/8/9.
//

#ifndef REACTOR_CONDITION_H
#define REACTOR_CONDITION_H

#include "Mutex.h"
#include <chrono>

namespace reactor::core {
    using std::chrono::nanoseconds;

    class Condition final : public NonCopyable {
    private:
        Mutex &mutex;
        pthread_cond_t cond;
    public:
        explicit Condition(Mutex &mutex);

        ~Condition();

        void wait();

        // wait a thread until a signal or timeout.
        // return true if timeout.
        bool timed_wait(nanoseconds ns);

        void notify();

        void notify_all();
    };
}

#endif //REACTOR_CONDITION_H
