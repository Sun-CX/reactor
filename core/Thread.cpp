//
// Created by suncx on 2020/8/8.
//

#include "Thread.h"
#include "Exception.h"
#include "CurrentThread.h"
#include <sys/prctl.h>
#include <syscall.h>
#include <cstring>

using std::move;
using std::to_string;

atomic_int Thread::thread_count;

pre_main void set_startup_process_tsd() {
    const char *main_thread_name = "main-thread";
    int status = prctl(PR_SET_NAME, main_thread_name);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    strcpy(CurrentThread::name, main_thread_name);
    CurrentThread::pid = getpid();
}

Thread::Thread(Thread::thread_func func, string name) : func(move(func)), name(move(name)), tid(0), pid(0) {
    // 对于 Linux 来说，进程 ID 为 0 是非法值，操作系统第一个进程 systemd 的 pid 是 1
    if (this->name.empty()) this->name = "thread-" + to_string(++thread_count);
}

void Thread::start() {
    int status = pthread_create(&tid, nullptr, thread_routine, this);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
}

void *Thread::thread_routine(void *arg) {
    auto th = static_cast<Thread *>(arg);
    int status = pthread_setname_np(th->tid, th->name.c_str());
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    th->pid = syscall(SYS_gettid);

    strcpy(CurrentThread::name, th->name.c_str());
    CurrentThread::pid = th->pid;

    th->func();
    return nullptr;
}

void Thread::join() {
    int status = pthread_join(tid, nullptr);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
}

const string &Thread::get_name() const {
    return this->name;
}

pid_t Thread::get_tid() const {
    return pid;
}