//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_THREAD_H
#define REACTOR_THREAD_H

#include "CountDownLatch.h"
#include <atomic>
#include <string>
#include <functional>

namespace reactor::core {
    using std::atomic_uint;
    using std::function;
    using std::string;

    class Thread final : public NonCopyable {
    private:
        using runnable = function<void()>;

        runnable func;
        // logic name of a thread.
        string thread_name;

        // POSIX thread id.
        pthread_t tid;

        // kernel light weight process id.
        pid_t pid;
        CountDownLatch latch;

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

    // Note: Do Not rewrite CurrentThread::name and CurrentThread::id, readonly for caller!
    class CurrentThread final : public NonCopyable {
    public:
        CurrentThread() = delete;

        // logic name of current thread, 16 bytes long(including null terminator) at most.
        thread_local static char name[16];
        // kernel light weight process id associated with a posix thread.
        thread_local static pid_t id;

        // check if current thread is start thread.
        static bool is_main_thread();

        // sleep current thread for `ms` milliseconds and `ns` nanoseconds.
        static int sleep(long ms, int ns = 0);
    };
}

#endif //REACTOR_THREAD_H
