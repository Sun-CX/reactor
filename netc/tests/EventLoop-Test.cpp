//
// Created by suncx on 2020/8/25.
//

#include "EventLoop.h"
#include "Thread.h"
#include "ConsoleStream.h"

using std::chrono::steady_clock;
using std::chrono::system_clock;
using std::chrono_literals::operator ""s;
using reactor::net::EventLoop;
using reactor::core::Thread;
using reactor::core::CurrentThread;

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

void eventloop_of_current_thread_test() {
    EventLoop *pev = EventLoop::eventloop_of_current_thread();
    if (pev == nullptr)
        RC_DEBUG << "nullptr";
    else
        RC_DEBUG << pev;

    EventLoop loop;
    pev = EventLoop::eventloop_of_current_thread();
    if (pev == nullptr)
        RC_DEBUG << "nullptr";
    else {
        RC_DEBUG << pev;
        RC_DEBUG << (pev == &loop);
    }

    Thread thread([] {
        EventLoop *pev = EventLoop::eventloop_of_current_thread();
        if (pev == nullptr)
            RC_DEBUG << "nullptr";
        else
            RC_DEBUG << pev;
    }, "loop-thread");
    thread.start();
    thread.join();
}

void quit_loop() {
    EventLoop loop;

    Thread thread([&loop] {
        CurrentThread::sleep(std::chrono::nanoseconds());
        loop.quit();
    }, "quit-loop");

    thread.start();

    loop.loop();
    thread.join();
}

void run_in_loop_test() {
    EventLoop loop;

    Thread thread([&loop] {
        CurrentThread::sleep(std::chrono::nanoseconds());

        loop.run_in_loop([&loop] {
            RC_DEBUG << "=============== run_in_loop2 ===============";
        });

        // CurrentThread::sleep(500);
        loop.quit();

    }, "run-loop");
    thread.start();

    loop.run_in_loop([] {
        RC_DEBUG << "=============== run_in_loop1 ===============";
    });

    loop.loop();
    thread.join();
}

void test1() {
    EventLoop loop;
    loop.schedule([&loop] {
        RC_DEBUG << "wo kao";
        // loop.quit();
    }, 0s, 2s);

    // loop.schedule([] {
    //     printf("hello\n");
    // }, 1_s, 1_s);
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
    // call_loop_in_another_thread();

    // eventloop_of_current_thread_test();

    // quit_loop();

    // run_in_loop_test();

    test1();

    return 0;
}