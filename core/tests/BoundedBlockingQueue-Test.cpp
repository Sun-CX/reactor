//
// Created by suncx on 2020/8/13.
//

#include "BoundedBlockingQueue.h"
#include "CountDownLatch.h"
#include "ConsoleStream.h"
#include "Thread.h"
#include <vector>
#include <memory>
#include <functional>

using std::string;
using std::vector;
using std::unique_ptr;
using std::bind;
using reactor::core::BoundedBlockingQueue;
using reactor::core::CountDownLatch;
using reactor::core::Thread;
using reactor::core::CurrentThread;

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
        RC_DEBUG << "start...";
        latch.count_down();
        bool running = true;
        while (running) {
            string d = queue.pop_front();

            RC_DEBUG << "consume data: " << d << ", size: " << queue.size();

            running = d != "stop";
        }
        RC_DEBUG << "stop...";
    }

public:
    explicit Test(int n_threads) : queue(20), latch(n_threads) {
        threads.reserve(n_threads);
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            ::snprintf(name, sizeof(name), "work-thread-%d", i + 1);
            threads.emplace_back(new Thread(bind(&Test::thread_func, this), name));
        }
        for (const auto &th : threads) th->start();
    }

    void run(int times) {
        RC_DEBUG << "waiting for count_down_latch...";
        latch.wait();
        RC_DEBUG << "all threads have started...";
        char buf[32];
        for (int i = 0; i < times; ++i) {
            ::snprintf(buf, sizeof(buf), "hello %d", i + 1);
            queue.push_back(buf);

            RC_DEBUG << "en_que: " << buf << ", size: " << queue.size();
        }
    }

    void join_all() {
        for (size_t i = 0; i < threads.size(); ++i)
            queue.push_back("stop");
        for (const auto &th:threads) th->join();
    }
};

int main(int argc, const char *argv[]) {

    Test t(5);
    t.run(100);
    t.join_all();

    return 0;
}