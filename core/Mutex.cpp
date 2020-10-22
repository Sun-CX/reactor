//
// Created by suncx on 2020/8/9.
//

#include "Mutex.h"
#include "Exception.h"
#include "ConsoleStream.h"

Mutex::Mutex() : pid(0) {
    auto status = pthread_mutex_init(&mutex, nullptr);
    if (unlikely(status != 0)) FATAL << "mutex init error!";
}

void Mutex::lock() {
    auto status = pthread_mutex_lock(&mutex);
    if (unlikely(status != 0)) FATAL << "mutex lock error!";
    pid = CurrentThread::pid;
}

void Mutex::unlock() {
    pid = 0;
    auto status = pthread_mutex_unlock(&mutex);
    if (unlikely(status != 0)) FATAL << "mutex unlock error!";
}

pthread_mutex_t *Mutex::get_mutex() {
    return &mutex;
}

bool Mutex::is_locked_by_cur_thread() const {
    return pid == CurrentThread::pid;
}

void Mutex::assert_locked_by_cur_thread() const {
    if (!is_locked_by_cur_thread()) FATAL << "lock assert failed!";
}

Mutex::~Mutex() {
    if (unlikely(pid != 0)) FATAL << "one thread holds this mutex, destroy error!";
    auto status = pthread_mutex_destroy(&mutex);
    if (unlikely(status != 0)) FATAL << "mutex destroy error!";
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
    mut.pid = CurrentThread::pid;
}