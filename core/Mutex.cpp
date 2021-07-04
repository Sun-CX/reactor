//
// Created by suncx on 2020/8/9.
//

#include "Mutex.h"
#include "Thread.h"
#include "GnuExt.h"
#include "ConsoleStream.h"

using reactor::core::Mutex;
using reactor::core::MutexGuard;

Mutex::Mutex() noexcept : pid(0) {
    auto status = pthread_mutex_init(&mutex, nullptr);
    if (unlikely(status != 0)) RC_FATAL << "mutex init error!";
}

Mutex::~Mutex() {
    if (unlikely(pid != 0)) RC_FATAL << "one thread holds this mutex, destroy error!";
    auto status = pthread_mutex_destroy(&mutex);
    if (unlikely(status != 0)) RC_FATAL << "mutex destroy error!";
}

void Mutex::lock() {
    auto status = pthread_mutex_lock(&mutex);
    if (unlikely(status != 0)) RC_FATAL << "mutex lock error!";
    pid = CurrentThread::id;
}

void Mutex::unlock() {
    pid = 0;
    auto status = pthread_mutex_unlock(&mutex);
    if (unlikely(status != 0)) RC_FATAL << "mutex unlock error!";
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