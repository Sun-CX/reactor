//
// Created by suncx on 2020/8/17.
//

#include "EventLoopDoNothing.h"
#include "Thread.h"

void thread_func() {
    printf("%s[%d] started...\n", CurrentThread::name, CurrentThread::pid);
    EventLoopDoNothing loop;
    loop.loop();
}

void test1() {
    printf("%s[%d] started...\n", CurrentThread::name, CurrentThread::pid);
    EventLoopDoNothing loop;

    Thread t(thread_func);
    t.start();

    loop.loop();
    t.join();
}

EventLoopDoNothing *g_loop;

void test2() {
    EventLoopDoNothing loop;
    g_loop = &loop;

    Thread t([]() -> void {
        g_loop->loop();
    });

    t.start();
    t.join();
}

int main(int argc, const char *argv[]) {

    test1();
//    test2();
    return 0;
}