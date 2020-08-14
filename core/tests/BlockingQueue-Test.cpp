//
// Created by suncx on 2020/8/10.
//
#include "../BlockingQueue.h"
#include "../CountDownLatch.h"
#include "../Thread.h"
#include <memory>
#include <vector>
#include <functional>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::bind;

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
        printf("%s[%d] started...\n", CurrentThread::name, CurrentThread::pid);
        latch.count_down();
        bool running = true;
        while (running) {
            string d = queue.de_queue();
            printf("%s[%d]: consume data: %s, size = %zu\n", CurrentThread::name,
                   CurrentThread::pid, d.c_str(), queue.size());
            running = d != "stop";
        }
        printf("%s[%d] stopped...\n", CurrentThread::name, CurrentThread::pid);
    }

public:
    explicit Test(int n_threads) : latch(n_threads) {
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            snprintf(name, sizeof(name), "work-thread-%d", i + 1);
            threads.emplace_back(new Thread(bind(&Test::thread_func, this), name));
        }
        for (const auto &e:threads) e->start();
    }

    void run(int times) {
        printf("waiting for count_down_latch...\n");
        latch.wait();
        printf("all threads have started...\n");
        char buf[32];
        for (int i = 0; i < times; ++i) {
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "hello %d", i + 1);
            queue.en_queue(buf);
            printf("%s[%d] enque: %s, queue size: %zu\n", CurrentThread::name, CurrentThread::pid,
                   buf, queue.size());
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

    printf("thread name: %s, pid: %d, tid: %d\n", CurrentThread::name, getpid(), CurrentThread::pid);

    Test t(5);   // 5 个消费者消费数据
    t.run(100);     // 主线程生产 100 条数据
    t.join_all();

//    test_move();

    return 0;
}