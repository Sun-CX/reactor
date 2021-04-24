//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_THREAD_H
#define REACTOR_THREAD_H

#include "NonCopyable.h"
#include "GnuExt.h"
#include <atomic>
#include <string>
#include <pthread.h>
#include <functional>

using std::atomic_uint;
using std::function;
using std::string;

class Thread final : public NonCopyable {
private:
    using thread_func = function<void()>;

    thread_func func;
    string name;
    pthread_t tid;      // POSIX 虚拟线程 ID
    pid_t pid;          // kernel 线程（LWP）真实 ID

    static atomic_uint thread_count;

    static void *thread_routine(void *arg);

public:
    explicit Thread(thread_func func, string name = "");

    void start();

    void join();

    [[nodiscard]]
    const string &get_name() const;

    [[nodiscard]]
    pid_t get_tid() const;
};

#endif //REACTOR_THREAD_H
