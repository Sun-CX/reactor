//
// Created by suncx on 2020/8/9.
//

#include "Mutex.h"

Mutex::Mutex() : pid(0) {
    auto status = pthread_mutex_init(&mutex, nullptr);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
}

void Mutex::lock() {
    auto status = pthread_mutex_lock(&mutex);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    pid = CurrentThread::pid;
}

void Mutex::unlock() {
    pid = 0;
    auto status = pthread_mutex_unlock(&mutex);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
}

pthread_mutex_t *Mutex::get_mutex() {
    return &mutex;
}

bool Mutex::is_locked_by_cur_thread() const {
    return pid == CurrentThread::pid;
}

void Mutex::assert_locked_by_cur_thread() const {
    if (!is_locked_by_cur_thread()) ERROR_EXIT("assert error.");
}

Mutex::~Mutex() {
    if (unlikely(pid != 0)) ERROR_EXIT("error occurred.");
    auto status = pthread_mutex_destroy(&mutex);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
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