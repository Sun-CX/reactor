//
// Created by suncx on 2020/8/13.
//

#include "BoundedBlockingQueue.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include <vector>
#include <memory>
#include <functional>

using std::vector;
using std::unique_ptr;
using std::bind;

/**
 * 本例是一个典型的生产者/消费者模型：
 * 由于使用了有界阻塞队列，生产者最多只能生产 20 个产品，然后等待消费者去消费
 * 因此无论生产者的生产效率有多么高，都不会发生占满内存的问题
 */
class Test {

private:
    BoundedBlockingQueue<string> queue;
    CountDownLatch latch;
    vector<unique_ptr<Thread>> threads;

    void thread_func() {
        printf("%s[%d] started...\n", CurrentThread::name, CurrentThread::pid);
        latch.count_down();
        bool running = true;
        while (running) {
            string d = queue.pop_front();
            printf("%s[%d]: consume data: %s, size = %zu\n", CurrentThread::name,
                   CurrentThread::pid, d.c_str(), queue.size());

            running = d != "stop";
        }
        printf("%s[%d] stopped...\n", CurrentThread::name, CurrentThread::pid);
    }

public:
    explicit Test(int n_threads) : queue(20), latch(n_threads) {
        threads.reserve(n_threads);
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            snprintf(name, sizeof(name), "work-thread-%d", i + 1);
            threads.emplace_back(new Thread(bind(&Test::thread_func, this), name));
        }
        for (const auto &th:threads) th->start();
    }

    void run(int times) {
        printf("waiting for count_down_latch...\n");
        latch.wait();
        printf("all threads have started...\n");
        char buf[32];
        for (int i = 0; i < times; ++i) {
            snprintf(buf, sizeof(buf), "hello %d", i + 1);
            queue.push_back(buf);
            printf("%s[%d] push_back: %s, queue size: %zu\n", CurrentThread::name,
                   CurrentThread::pid,
                   buf, queue.size());
        }
    }

    void join_all() {
        for (size_t i = 0; i < threads.size(); ++i) {
            queue.push_back("stop");
        }
        for (const auto &th:threads) th->join();
    }
};

int main(int argc, const char *argv[]) {

    Test t(5);
    t.run(100);
    t.join_all();

    return 0;
}