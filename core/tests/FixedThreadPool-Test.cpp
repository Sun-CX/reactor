//
// Created by suncx on 2020/8/15.
//

#include "Thread.h"
#include "FixedThreadPool.h"
#include "CountDownLatch.h"
#include <functional>

using std::bind;
using reactor::core::CurrentThread;
using reactor::core::CountDownLatch;
using reactor::core::FixedThreadPool;

static void print() {
    printf("%s[%d] execute print...\n", CurrentThread::name, CurrentThread::id);
}

static void print_str(const string &str) {
    printf("%s[%d] execute task: %s\n", CurrentThread::name, CurrentThread::id, str.c_str());
//    usleep(100 * 1000);
}

/**
 * 基本的开启和关闭
 */
static void test1() {
    FixedThreadPool pool(5, 10, "reactor-pool");
    pool.start();
    pool.shutdown();
}

/**
 * 在 start() 之前提交任务：则全部任务都在 main 线程中运行
 */
static void test2() {
    FixedThreadPool pool(5, 10, "reactor-pool");

    char task_no[32];
    for (int i = 0; i < 100; ++i) {
        snprintf(task_no, sizeof(task_no), "task-%d", i + 1);
        pool.submit(bind(print_str, string(task_no)));
    }
    printf("Tasks submit done.\n");
    pool.start();
    pool.shutdown();
}

static void test3() {
    FixedThreadPool pool(5, 10, "reactor-pool");
    pool.start();

    char task_no[32];
    for (int i = 0; i < 100; ++i) {
        snprintf(task_no, sizeof(task_no), "task-%d", i + 1);
        pool.submit(bind(print_str, string(task_no)));
    }
    printf("Tasks submit done.\n");
    CountDownLatch latch(1);
    pool.submit(bind(&CountDownLatch::count_down, &latch));
    latch.wait();
    pool.shutdown();
}

int main(int argc, const char *argv[]) {

//    test1();
//    test2();
    test3();

    return 0;
}