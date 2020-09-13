//
// Created by suncx on 2020/8/25.
//

#include "EventLoop.h"
#include "Thread.h"

static EventLoop *g_loop;

void test1() {
    EventLoop loop;
    loop.schedule([&loop] {
        printf("wokao\n");
        loop.quit();
    }, 6_s);

    loop.schedule([] {
        printf("hello\n");
    }, 1_s, 1_s);
    loop.loop();
}

void test2() {
    EventLoop loop;
    g_loop = &loop;


    Thread thread([] {
        g_loop->loop();
    });

    thread.start();
    thread.join();
}

int main(int argc, const char *argv[]) {

    test1();
//    test2();
    return 0;
}