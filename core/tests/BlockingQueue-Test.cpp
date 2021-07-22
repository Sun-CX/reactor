//
// Created by suncx on 2020/8/10.
//

#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include "ConsoleStream.h"
#include "Thread.h"
#include <memory>
#include <vector>

using std::string;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::bind;
using reactor::core::BlockingQueue;
using reactor::core::CountDownLatch;
using reactor::core::Thread;
using reactor::core::CurrentThread;

/**
 * 本例是一个典型的生产者/消费者模型：
 * 主线程生产数据，5 个子线程消费数据
 */
class Test {
private:
    BlockingQueue<string> queue;
    CountDownLatch latch;
    vector<unique_ptr<Thread>> threads;

    void thread_func() {
        RC_DEBUG << "start...";
        latch.count_down();
        bool running = true;
        while (running) {
            string d = queue.de_queue();

            RC_DEBUG << "consume data: " << d << ", size: " << queue.size();

            running = d != "stop";
        }
        RC_DEBUG << "stop...";
    }

public:
    explicit Test(int n_threads) : latch(n_threads) {
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            ::snprintf(name, sizeof(name), "work-thread-%d", i + 1);
            threads.emplace_back(new Thread(bind(&Test::thread_func, this), name));
        }
        for (const auto &e : threads) e->start();
    }

    void run(int times) {
        RC_DEBUG << "waiting for count_down_latch...";
        latch.wait();
        RC_DEBUG << "all threads have started...";
        char buf[32];
        for (int i = 0; i < times; ++i) {
            ::snprintf(buf, sizeof(buf), "hello %d", i + 1);
            queue.en_queue(buf);
            RC_DEBUG << "en_que: " << buf << ", queue's size: " << queue.size();
        }
    }

    void join_all() {
        for (size_t i = 0; i < threads.size(); ++i) {
            queue.en_queue("stop");
        }
        for (const auto &th:threads) th->join();
    }
};

void test_move() {
    BlockingQueue<unique_ptr<int>> queue;
    queue.en_queue(make_unique<int>(42));

    unique_ptr<int> ptr = queue.de_queue();
    printf("de_que value: %d\n", *ptr);

    *ptr = 123;
    queue.en_queue(move(ptr));
    printf("judge: %d\n", ptr == nullptr);

    unique_ptr<int> y = queue.de_queue();
    printf("deque value2: %d\n", *y);
}

int main(int argc, const char *argv[]) {

    Test t(5);   // 5 个消费者消费数据
    t.run(100);     // 主线程生产 100 条数据
    t.join_all();

    return 0;
}