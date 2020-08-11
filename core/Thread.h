//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_THREAD_H
#define REACTOR_THREAD_H

#include <pthread.h>
#include <string>
#include <atomic>
#include <functional>
#include <unistd.h>
#include <syscall.h>
#include <sys/prctl.h>
#include "NonCopyable.h"
#include "Exception.h"

#define pre_main __attribute__((constructor))

using std::string;
using std::atomic_int;
using std::function;
using std::move;
using std::to_string;

class Thread final : public NonCopyable {
private:
    using thread_func = function<void()>;

    friend class CurrentThread;

    friend void set_startup_process_tsd();

    struct TSD {
        string name;
        pid_t pid;      // kernel 线程（LWP）真实 ID
        TSD(string name, pid_t pid);
    };

    thread_func func;
    pthread_t tid;      // POSIX 虚拟线程 ID
    TSD tsd;

    static atomic_int thread_count;
    static pthread_key_t key;

    static void *thread_routine(void *arg);

public:
    explicit Thread(thread_func func, string name = "");

    void start();

    void join();

    const string &get_name() const;

    pid_t get_tid() const;
};

#endif //REACTOR_THREAD_H
