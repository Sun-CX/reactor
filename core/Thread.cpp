//
// Created by suncx on 2020/8/8.
//

#include "Thread.h"
#include "GnuExt.h"
#include "ConsoleStream.h"
#include <sys/prctl.h>
#include <syscall.h>
#include <cstring>
#include <unistd.h>
#include <cassert>

using std::string;
using std::move;
using std::to_string;
using std::atomic_uint;
using reactor::core::Thread;
using reactor::core::CurrentThread;

atomic_uint Thread::thread_count(0);

attr_constructor void main_thread_initialize() {
    const char *main_proc_name = "main-thread";
    if (unlikely(::prctl(PR_SET_NAME, main_proc_name) < 0))
        RC_FATAL << "prctl error: " << strerror(errno);
    ::strcpy(CurrentThread::name, main_proc_name);
    CurrentThread::id = ::getpid();
}

Thread::Thread(runnable func, const char *name) :
        func(move(func)),
        tid(0),
        pid(0),
        latch(1) {

    assert(this->func != nullptr);

    if (name == nullptr or ::strlen(name) == 0) {
        uint32_t cnt = ++thread_count;
        ::snprintf(thread_name, sizeof(thread_name), "thread-%u", cnt);
    } else {
        ::snprintf(thread_name, sizeof(thread_name), "%s", name);
    }
}


Thread::Thread(runnable func, const string &name) :
        func(move(func)),
        tid(0),
        pid(0),
        latch(1) {

    assert(this->func != nullptr);

    if (name.empty()) {
        uint32_t cnt = ++thread_count;
        ::snprintf(thread_name, sizeof(thread_name), "thread-%u", cnt);
    } else {
        ::snprintf(thread_name, sizeof(thread_name), "%s", name.c_str());
    }
}

void Thread::start() {
    int ret = ::pthread_create(&tid, nullptr, thread_routine, this);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread create error: " << ret;
    latch.wait();
}

void *Thread::thread_routine(void *arg) {
    auto self = static_cast<Thread *>(arg);

    int ret = ::pthread_setname_np(self->tid, self->thread_name);

    if (unlikely(ret != 0))
        RC_FATAL << "pthread set name error: " << ret;

    self->pid = ::syscall(SYS_gettid);
    self->latch.count_down();

    ::strncpy(CurrentThread::name, self->thread_name, sizeof(CurrentThread::name));
    CurrentThread::id = self->pid;

    self->func();
    return nullptr;
}

void Thread::join() {
    int ret = ::pthread_join(tid, nullptr);
    if (unlikely(ret != 0))
        RC_FATAL << "pthread join error: " << ret;
}

const char *Thread::get_name() const {
    return thread_name;
}

pid_t Thread::get_id() const {
    return pid;
}

thread_local char CurrentThread::name[16];

thread_local pid_t CurrentThread::id;

bool CurrentThread::is_main_thread() {
    return id == ::getpid();
}

int CurrentThread::sleep(long ms, int ns) {
    if (ms < 0 or ns < 0 or ns > 999999) {
        RC_FATAL << "sleep time out of range.";
    }

    timespec time;
    time.tv_sec = ms / 1000;
    time.tv_nsec = ms % 1000 * 1000 * 1000 + ns;

    return ::nanosleep(&time, nullptr);
}