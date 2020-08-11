//
// Created by suncx on 2020/8/8.
//

#include "Thread.h"

atomic_int Thread::thread_count;
pthread_key_t Thread::key;

pre_main void set_startup_process_tsd() {
    const char *main_thread_name = "main-thread";
    int status = prctl(PR_SET_NAME, main_thread_name);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");

    auto tsd = new Thread::TSD(main_thread_name, getpid());
    status = pthread_key_create(&Thread::key, nullptr);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");

    status = pthread_setspecific(Thread::key, tsd);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
}

Thread::Thread(Thread::thread_func func, string name) : func(move(func)), tid(0), tsd(move(name), 0) {}

void Thread::start() {
    if (tsd.name.empty()) tsd.name = "thread-" + to_string(++thread_count);
    int status = pthread_create(&tid, nullptr, thread_routine, this);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
}

void *Thread::thread_routine(void *arg) {
    auto th = static_cast<Thread *>(arg);
    int status = pthread_setname_np(th->tid, th->tsd.name.c_str());
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");

    th->tsd.pid = syscall(SYS_gettid);
    status = pthread_setspecific(key, &th->tsd);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    th->func();
    return nullptr;
}

void Thread::join() {
    int status = pthread_join(tid, nullptr);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
}

const string &Thread::get_name() const {
    return tsd.name;
}

pid_t Thread::get_tid() const {
    return tsd.pid;
}

Thread::TSD::TSD(string name, pid_t pid) : name(move(name)), pid(pid) {}
