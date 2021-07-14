//
// Created by suncx on 2020/8/9.
//

#include "Mutex.h"
#include "Thread.h"
#include "Ext.h"
#include "ConsoleStream.h"

#ifdef __linux__

#include <cstring>

#endif

using reactor::core::Mutex;
using reactor::core::MutexGuard;

Mutex::Mutex() noexcept : pid(0) {
    int ret = ::pthread_mutex_init(&mutex, nullptr);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread mutex init error:" << ::strerror(ret);
}

Mutex::~Mutex() {
    if (unlikely(pid != 0))
        RC_FATAL << "one thread(" << pid << ") holds this mutex, destroy error";

    int ret = ::pthread_mutex_destroy(&mutex);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread mutex destroy error: " << ::strerror(ret);
}

void Mutex::lock() {
    int ret = ::pthread_mutex_lock(&mutex);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread mutex lock error: " << ::strerror(ret);
    pid = CurrentThread::id;
}

void Mutex::unlock() {
    pid = 0;
    int ret = ::pthread_mutex_unlock(&mutex);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread mutex unlock error: " << ::strerror(ret);
}

pthread_mutex_t *Mutex::get_mutex() {
    return &mutex;
}

bool Mutex::is_locked_by_cur_thread() const {
    return pid == CurrentThread::id;
}

MutexGuard::MutexGuard(Mutex &mutex) : mutex(mutex) {
    this->mutex.lock();
}

MutexGuard::~MutexGuard() {
    mutex.unlock();
}

Mutex::ConditionWaitGuard::ConditionWaitGuard(Mutex &mut) : mut(mut) {
    this->mut.pid = 0;
}

Mutex::ConditionWaitGuard::~ConditionWaitGuard() {
    mut.pid = CurrentThread::id;
}