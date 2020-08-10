//
// Created by suncx on 2020/8/9.
//

#include "../CountDownLatch.h"
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <cstring>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::bind;
using std::for_each;
using std::placeholders::_1;
using std::move;

class Test {
private:
    CountDownLatch latch;
    vector<unique_ptr<Thread>> threads;

    void thread_func() {
        latch.wait();
        printf("tid: %d, %s started...\n", CurrentThread::get_pid(), CurrentThread::get_name().c_str());
        printf("tid: %d, %s stopped...\n", CurrentThread::get_pid(), CurrentThread::get_name().c_str());
    }

public:
    explicit Test(int n_threads) : latch(1) {
        threads.reserve(n_threads);
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "work thread-%d", i);
            threads.push_back(make_unique<Thread>(bind(&Test::thread_func, this), name));
        }
//        for_each(threads.cbegin(), threads.cend(), [](const unique_ptr<Thread> &ptr) {
//            ptr->start();
//        });
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
    printf("pid: %d, tid: %d\n", getpid(), CurrentThread::get_pid());

    Test t(3);
//    sleep(3);
    printf("pid: %d, tid: %d, %s running...\n", getpid(), CurrentThread::get_pid(), CurrentThread::get_name().c_str());
    t.run();
    t.join_all();

//    vector<int> v;
//    printf("vector default size:%zu, capacity: %zu\n", v.size(), v.capacity());
    return 0;
}