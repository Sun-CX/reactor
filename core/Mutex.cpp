//
// Created by suncx on 2020/8/9.
//

#include "Mutex.h"

Mutex::Mutex() : pid(0) {
    assert(pthread_mutex_init(&mutex, nullptr) == 0);
}

void Mutex::lock() {
    assert(pthread_mutex_lock(&mutex) == 0);
    pid = CurrentThread::get_pid();
}

void Mutex::unlock() {
    pid = 0;
    assert(pthread_mutex_unlock(&mutex) == 0);
}

pthread_mutex_t *Mutex::get_mutex() {
    return &mutex;
}

bool Mutex::is_locked_by_cur_thread() const {
    return pid == CurrentThread::get_pid();
}

void Mutex::assert_locked_by_cur_thread() const {
    assert(is_locked_by_cur_thread());
}

Mutex::~Mutex() {
    assert(pid == 0);
    assert(pthread_mutex_destroy(&mutex) == 0);
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
    mut.pid = CurrentThread::get_pid();
}