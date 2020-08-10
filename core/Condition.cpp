//
// Created by suncx on 2020/8/9.
//

#include "Condition.h"

Condition::Condition(Mutex &mutex) : mutex(mutex) {
    assert(pthread_cond_init(&cond, nullptr) == 0);
}

Condition::~Condition() {
    assert(pthread_cond_destroy(&cond) == 0);
}

void Condition::wait() {
    Mutex::ConditionWaitGuard guard(mutex);
    assert(pthread_cond_wait(&cond, mutex.get_mutex()) == 0);
}

bool Condition::timed_wait(double seconds) {
    timespec ts;
    assert(clock_gettime(CLOCK_REALTIME, &ts) == 0);
    //TODO: seconds is double, not int
    ts.tv_sec += seconds;

    Mutex::ConditionWaitGuard guard(mutex);
    return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get_mutex(), &ts);
}

void Condition::notify() {
    assert(pthread_cond_signal(&cond) == 0);
}

void Condition::notify_all() {
    assert(pthread_cond_broadcast(&cond) == 0);
}

