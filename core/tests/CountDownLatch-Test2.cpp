//
// Created by suncx on 2020/8/10.
//

#include "CountDownLatch.h"
#include "Thread.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <unistd.h>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::bind;
using std::for_each;
using std::placeholders::_1;
using reactor::core::CountDownLatch;
using reactor::core::Thread;
using reactor::core::CurrentThread;

/**
 * 本例演示了：
 * 主线程等待所有子线程执行完毕后再继续执行
 */

class Demo {
private:

    CountDownLatch latch;
    vector<unique_ptr<Thread>> threads;

    void thread_func() {
        printf("tid: %d, %s started...\n", CurrentThread::id, CurrentThread::name);
//        sleep(3);
        printf("tid: %d, %s stopped...\n", CurrentThread::id, CurrentThread::name);
        latch.count_down();
    }

public:
    explicit Demo(int n_threads) : latch(n_threads) {
        threads.reserve(n_threads);
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            snprintf(name, sizeof(name), "work-thread-%d", i + 1);
            threads.push_back(make_unique<Thread>(bind(&Demo::thread_func, this), name));
        }
        for_each(threads.cbegin(), threads.cend(), bind(&Thread::start, _1));
    }

    void wait() {
        latch.wait();
    }

    void join_all() {
        for_each(threads.cbegin(), threads.cend(), bind(&Thread::join, _1));
    }
};

int main(int argc, const char *argv[]) {
    printf("pid: %d, tid: %d\n", getpid(), CurrentThread::id);

    Demo demo(3);
    demo.wait();

    printf("pid: %d, tid: %d %s running...\n", getpid(), CurrentThread::id, CurrentThread::name);

    demo.join_all();

    return 0;
}