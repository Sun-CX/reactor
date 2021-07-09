//
// Created by suncx on 2020/8/25.
//

#include "EventLoop.h"
#include "Thread.h"

using reactor::net::EventLoop;
using reactor::core::Thread;
using reactor::core::operator ""_s;

void one_event_loop_in_main_thread() {
    EventLoop loop;
}

void multi_event_loop_in_main_thread() {
    EventLoop loop;
    EventLoop loop1;
}

void call_loop_in_another_thread() {
    EventLoop loop;

    Thread thread([&loop]() {
        loop.loop();
    }, "loop-thread");

    thread.start();
    thread.join();
}

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

    Thread thread([&loop] {
        loop.loop();
    });

    thread.start();
    thread.join();
}

int main(int argc, const char *argv[]) {

    // one_event_loop_in_main_thread();
    // multi_event_loop_in_main_thread();

    call_loop_in_another_thread();
//    test1();
//    test2();

    return 0;
}