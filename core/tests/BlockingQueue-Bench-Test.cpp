//
// Created by suncx on 2020/8/12.
//

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "../Thread.h"
#include "../Timestamp.h"
#include "../BlockingQueue.h"
#include "../CountDownLatch.h"

using std::vector;
using std::unique_ptr;
using std::bind;
using std::map;

/**
 * 本例为一个统计程序：
 * 统计的是从生产者生产数据（Timestamp）到消费者消费数据，所耗费的微秒数及其次数
 */
class Bench {
private:
    vector<unique_ptr<Thread>> threads;
    BlockingQueue<Timestamp> queue;
    CountDownLatch latch;

    void thread_func() {
        printf("%s[%d] started...\n", CurrentThread::name, CurrentThread::pid);
        latch.count_down();
        map<int, int> delays;
        bool running = true;
        while (running) {
            Timestamp timestamp = queue.de_queue();
            Timestamp now = Timestamp::now();
            if (timestamp.valid()) {
                // 时间差，以微秒计
                int delay = static_cast<int> (time_diff(now, timestamp) * 1000000);
                ++delays[delay];
            }
            running = timestamp.valid();
        }

        printf("%s[%d] stopped...\n", CurrentThread::name, CurrentThread::pid);

        for (const auto &delay:delays) {
            printf("%s[%d] delay: %d, count: %d\n", CurrentThread::name, CurrentThread::pid,
                   delay.first, delay.second);
        }
    }

public:
    explicit Bench(int n_threads) : latch(n_threads) {
        threads.reserve(n_threads);
        char name[32];
        for (int i = 0; i < n_threads; ++i) {
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "work-thread-%d", i + 1);
            threads.emplace_back(new Thread(bind(&Bench::thread_func, this), name));
        }
        for (const auto &th:threads) th->start();
    }

    void run(int times) {
        printf("waiting for count_down_latch...\n");
        latch.wait();
        printf("all threads have started...\n");
        for (int i = 0; i < times; ++i) {
            queue.en_queue(Timestamp::now());
            usleep(1000);   // 休眠 1000 微秒（1 毫秒）
        }
    }

    void join_all() {
        for (size_t i = 0; i < threads.size(); ++i) {
            queue.en_queue(Timestamp(0));
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