//
// Created by suncx on 2020/8/12.
//

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <unistd.h>
#include "Thread.h"
#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include "ConsoleStream.h"

using std::vector;
using std::unique_ptr;
using std::bind;
using std::map;
using std::chrono::duration_cast;
using std::chrono::system_clock;
using std::chrono::microseconds;
using std::chrono::nanoseconds;
using std::chrono_literals::operator ""ms;
using reactor::core::BlockingQueue;
using reactor::core::Thread;
using reactor::core::CountDownLatch;
using reactor::core::CurrentThread;

/**
 * 本例为一个统计程序：
 * 统计的是从生产者生产数据（Timestamp）到消费者消费数据，所耗费的微秒数及其次数
 */
class Bench {
private:
    vector<unique_ptr<Thread>> threads;
    BlockingQueue<nanoseconds> queue;
    CountDownLatch latch;

    void thread_func() {

        RC_DEBUG << "started..";

        latch.count_down();
        map<int, int> delays;
        bool running = true;
        while (running) {
            nanoseconds value = queue.de_queue();
            nanoseconds now = system_clock::now().time_since_epoch();

            if (value.count() > 0) {
                // 时间差，以微秒计
                auto delay = duration_cast<microseconds>(now - value).count();
                ++delays[delay];
            }
            running = value.count() > 0;
        }

        RC_DEBUG << "stopped...";

        for (const auto &delay : delays) {
            RC_DEBUG << "delay: " << delay.first << ", count: " << delay.second;
        }
    }

public:
    explicit Bench(int n_threads) : latch(n_threads) {
        threads.reserve(n_threads);
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            ::snprintf(name, sizeof(name), "work-thread-%d", i + 1);
            threads.emplace_back(new Thread(bind(&Bench::thread_func, this), name));
        }
        for (const auto &th : threads) th->start();
    }

    void run(int times) {
        RC_DEBUG << "waiting for count_down_latch...";
        latch.wait();
        RC_DEBUG << "all threads have started...";
        for (int i = 0; i < times; ++i) {
            queue.en_queue(system_clock::now().time_since_epoch());
            CurrentThread::sleep(1ms);
        }
    }

    void join_all() {
        for (size_t i = 0; i < threads.size(); ++i) {
            queue.en_queue(nanoseconds(0));
        }
        for (const auto &th:threads) th->join();
    }
};

int main(int argc, const char *argv[]) {

    Bench bench(5);
    bench.run(10000);
    bench.join_all();

    return 0;
}