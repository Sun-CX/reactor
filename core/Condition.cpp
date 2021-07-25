//
// Created by suncx on 2020/8/9.
//

#include "Condition.h"
#include "Ext.h"
#include "ConsoleStream.h"

#ifdef __linux__

#include <cstring>

#elif __APPLE__

#include <TargetConditionals.h>

#if TARGET_OS_MAC && TARGET_OS_OSX

#include <cerrno>

#endif

#endif

using reactor::core::Condition;
using std::chrono::system_clock;
using std::chrono_literals::operator ""s;

Condition::Condition(Mutex &mutex) : mutex(mutex) {
    int ret = ::pthread_cond_init(&cond, nullptr);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread cond init error: " << ::strerror(ret);
}

Condition::~Condition() {
    int ret = ::pthread_cond_destroy(&cond);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread cond destroy error: " << ::strerror(ret);
}

void Condition::wait() {
    int ret;
    Mutex::ConditionWaitGuard guard(mutex);
    ret = ::pthread_cond_wait(&cond, mutex.get_mutex());
    if (unlikely(ret != 0))
        RC_FATAL << "pthread cond wait error: " << ::strerror(ret);
}

bool Condition::timed_wait(const nanoseconds ns) {
    system_clock::time_point when = system_clock::now();
    when += ns;

    timespec ts;
    ts.tv_sec = when.time_since_epoch() / 1s;
    ts.tv_nsec = (when.time_since_epoch() % 1s).count();

    Mutex::ConditionWaitGuard guard(mutex);
    return ::pthread_cond_timedwait(&cond, mutex.get_mutex(), &ts) == ETIMEDOUT;
}

void Condition::notify() {
    int ret = ::pthread_cond_signal(&cond);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread cond signal error: " << ::strerror(ret);
}

void Condition::notify_all() {
    int ret = ::pthread_cond_broadcast(&cond);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread cond broadcast error: " << ::strerror(ret);
}

