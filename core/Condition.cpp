//
// Created by suncx on 2020/8/9.
//

#include "Condition.h"
#include "GnuExt.h"
#include "ConsoleStream.h"

using reactor::core::Condition;

Condition::Condition(Mutex &mutex) : mutex(mutex) {
    int status = pthread_cond_init(&cond, nullptr);
    if (unlikely(status != 0)) FATAL << "condition-variable init error.";
}

Condition::~Condition() {
    int status = pthread_cond_destroy(&cond);
    if (unlikely(status != 0)) FATAL << "condition-variable destroy error.";
}

void Condition::wait() {
    int status;
    Mutex::ConditionWaitGuard guard(mutex);
    status = pthread_cond_wait(&cond, mutex.get_mutex());
    if (unlikely(status != 0)) FATAL << "condition-variable wait error.";
}

bool Condition::timed_wait(long seconds, long microseconds) {
    if (seconds < 0 or microseconds < 0 or microseconds > 999999) {
        FATAL << "condition-variable wait timeout value out of range.";
    }

    timespec ts;
    int status = clock_gettime(CLOCK_REALTIME, &ts);
    if (unlikely(status != 0)) FATAL << "clock_gettime error.";

    ts.tv_sec += seconds;
    ts.tv_nsec += microseconds * 1000;

    Mutex::ConditionWaitGuard guard(mutex);
    return pthread_cond_timedwait(&cond, mutex.get_mutex(), &ts) == ETIMEDOUT;
}

void Condition::notify() {
    int status = pthread_cond_signal(&cond);
    if (unlikely(status != 0)) FATAL << "condition-variable signal error.";
}

void Condition::notify_all() {
    int status = pthread_cond_broadcast(&cond);
    if (unlikely(status != 0)) FATAL << "condition-variable broadcast error.";
}

