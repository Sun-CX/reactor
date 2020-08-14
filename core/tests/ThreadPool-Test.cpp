//
// Created by suncx on 2020/8/14.
//

#include "../ThreadPool.h"
#include "../CountDownLatch.h"

static void print() {
    printf("%s[%d] execute print...\n", CurrentThread::name, CurrentThread::pid);
}

static void print_str(const string &str) {
    printf("%s[%d] execute task: %s\n", CurrentThread::name, CurrentThread::pid, str.c_str());
    usleep(100 * 1000);
}

static void test(int max_size) {
    ThreadPool pool("reactor-pool");
    pool.set_max_queue_size(max_size);
    pool.start(5);

    pool.submit(print);
    pool.submit(print);

    char name[32];
    for (int i = 0; i < 100; ++i) {
        snprintf(name, sizeof(name), "task-%d", i + 1);
//        pool.submit(bind(print_str, name));
        pool.submit(bind(print_str, string(name)));
    }
    printf("Tasks submit done.\n");

    CountDownLatch latch(1);
    pool.submit(bind(&CountDownLatch::count_down, &latch));
    latch.wait();
    pool.stop();

}

int main(int argc, const char *argv[]) {

    test(10);

    return 0;
}