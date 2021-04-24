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

using std::atomic_uint;
using std::function;
using std::string;

class Thread final : public NonCopyable {
private:
    using runnable = function<void()>;

    runnable func;
    string thread_name;
    pthread_t tid;      // POSIX 虚拟线程 ID
    pid_t pid;          // kernel 线程（LWP）真实 ID

    static atomic_uint thread_count;

    static void *thread_routine(void *arg);

public:
    explicit Thread(runnable func, string name = "");

    void start();

    void join();

    [[nodiscard]]
    const string &name() const;

    [[nodiscard]]
    pid_t getid() const;
};

class CurrentThread final : public NonCopyable {
public:
    CurrentThread() = delete;

    /* 线程名最长为 16 字节（包括末尾 '\0' 符） */
    thread_local static char name[16];
    thread_local static pid_t id;
};

#endif //REACTOR_THREAD_H
