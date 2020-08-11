//
// Created by suncx on 2020/8/9.
//

#ifndef REACTOR_CONDITION_H
#define REACTOR_CONDITION_H

#include "NonCopyable.h"
#include "Exception.h"
#include "Mutex.h"

class Condition final : public NonCopyable {
private:
    Mutex &mutex;
    pthread_cond_t cond;
public:
    explicit Condition(Mutex &mutex);

    void wait();

    bool timed_wait(double seconds);

    void notify();

    void notify_all();

    ~Condition();
};

#endif //REACTOR_CONDITION_H
