//
// Created by suncx on 2020/8/8.
//

#include "CurrentThread.h"

const string &CurrentThread::get_name() {
    auto ptr = static_cast<Thread::TSD *>(pthread_getspecific(Thread::key));
    assert(ptr != nullptr);
    return ptr->name;
}

pid_t CurrentThread::get_pid() {
    auto ptr = static_cast<Thread::TSD *>(pthread_getspecific(Thread::key));
    assert(ptr != nullptr);
    return ptr->pid;
}


