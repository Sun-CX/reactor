//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_THREAD_H
#define REACTOR_THREAD_H

#include "NonCopyable.h"
#include <atomic>
#include <string>
#include <pthread.h>
#include <functional>

#define pre_main __attribute__((constructor))

using std::atomic_int;
using std::function;
using std::string;

class Thread final : public NonCopyable {
private:
    using thread_func = function<void()>;

    thread_func func;
    pthread_t tid;      // POSIX 虚拟线程 ID
    string name;
    pid_t pid;          // kernel 线程（LWP）真实 ID

    static atomic_int thread_count;

    static void *thread_routine(void *arg);

public:
    explicit Thread(thread_func func, string name = "");

    void start();

    void join();

    const string &get_name() const;

    pid_t get_tid() const;
};

#endif //REACTOR_THREAD_H
