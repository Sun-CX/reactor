//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_THREAD_H
#define REACTOR_THREAD_H

#include "CountDownLatch.h"
#include <atomic>
#include <string>
#include <functional>

using std::atomic_uint;
using std::function;
using std::string;

namespace reactor::core {
    class Thread final : public NonCopyable {
    private:
        using runnable = function<void()>;

        runnable func;
        string thread_name;
        pthread_t tid;      // POSIX 虚拟线程 ID
        pid_t pid;          // kernel 线程（LWP）真实 ID
        CountDownLatch latch;

        static atomic_uint thread_count;

        static void *thread_routine(void *arg);

    public:
        explicit Thread(runnable func, string name = "");

        void start();

        void join();

        const string &name() const;

        pid_t getid() const;
    };

/* Note: Do Not rewrite CurrentThread::name and CurrentThread::id. Readonly for caller! */
    class CurrentThread final : public NonCopyable {
    public:
        CurrentThread() = delete;

        /* 线程名最长为 16 字节（包括末尾 '\0' 符） */
        thread_local static char name[16];
        thread_local static pid_t id;

        static bool is_main_thread();

        static int sleep(long ms, int ns = 0);
    };
}

#endif //REACTOR_THREAD_H
