//
// Created by suncx on 2020/8/9.
//

#include "CountDownLatch.h"
#include "Thread.h"
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::bind;
using std::for_each;
using std::placeholders::_1;
using std::move;

/**
 * 本例演示了：
 * 主线程发起起跑命令，然后各子线程同时启动
 */

class Test {
private:
    CountDownLatch latch;
    vector<unique_ptr<Thread>> threads;

    void thread_func() {
        latch.wait();
        printf("tid: %d, %s started...\n", CurrentThread::pid, CurrentThread::name);
        printf("tid: %d, %s stopped...\n", CurrentThread::pid, CurrentThread::name);
    }

public:
    explicit Test(int n_threads) : latch(1) {
        threads.reserve(n_threads);
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            snprintf(name, sizeof(name), "work thread-%d", i);
            threads.push_back(make_unique<Thread>(bind(&Test::thread_func, this), name));
        }
        for_each(threads.cbegin(), threads.cend(), bind(&Thread::start, _1));
    }

    void run() {
        latch.count_down();
    }

    void join_all() {
        for_each(threads.begin(), threads.end(), bind(&Thread::join, _1));
    }
};

int main(int argc, const char *argv[]) {
    printf("pid: %d, tid: %d\n", getpid(), CurrentThread::pid);

    Test t(3);
//    sleep(3);
    printf("pid: %d, tid: %d, %s running...\n", getpid(), CurrentThread::pid, CurrentThread::name);
    printf("main thread allow rush...\n");
    t.run();
    t.join_all();

    return 0;
}