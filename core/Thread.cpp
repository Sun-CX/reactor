//
// Created by suncx on 2020/8/8.
//

#include "Thread.h"
#include "GnuExt.h"
#include "Exception.h"
#include "ConsoleStream.h"
#include <sys/prctl.h>
#include <syscall.h>
#include <cstring>
#include <unistd.h>

using std::move;
using std::to_string;

atomic_uint Thread::thread_count;

attr_constructor void main_thread_initialize() {
    const char *main_proc_name = "main-thread";
    int status = prctl(PR_SET_NAME, main_proc_name);
    if (unlikely(status != 0))
        FATAL << "prctl error!";
    strcpy(CurrentThread::name, main_proc_name);
    CurrentThread::id = getpid();
}

Thread::Thread(Thread::runnable func, string name) : func(move(func)), thread_name(move(name)), tid(0), pid(0) {
    // 对于 Linux 来说，进程 ID 为 0 是非法值，操作系统第一个进程 systemd 的 pid 是 1
    if (this->thread_name.empty()) this->thread_name = "thread-" + to_string(++thread_count);
}

void Thread::start() {
    int status = pthread_create(&tid, nullptr, thread_routine, this);
    if (unlikely(status != 0)) FATAL << "thread create error!";
}

void *Thread::thread_routine(void *arg) {
    auto self = static_cast<Thread *>(arg);

    int status = pthread_setname_np(self->tid, self->thread_name.c_str());

    if (unlikely(status != 0))
        FATAL << "set thread thread_name error!";

    self->pid = syscall(SYS_gettid);

    strcpy(CurrentThread::name, self->thread_name.c_str());
    CurrentThread::id = self->pid;

    self->func();
    return nullptr;
}

void Thread::join() {
    int status = pthread_join(tid, nullptr);
    if (unlikely(status != 0)) FATAL << "thread join error!";
}

const string &Thread::name() const {
    return this->thread_name;
}

pid_t Thread::getid() const {
    return pid;
}

thread_local char CurrentThread::name[16];

thread_local pid_t CurrentThread::id;

bool CurrentThread::is_main_thread() {
    return id == getpid();
}