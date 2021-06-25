//
// Created by suncx on 2020/9/5.
//

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "ConsoleStream.h"

using std::bind;

static void print(EventLoop *p = nullptr) {
    DEBUG << "loop: " << p;
}

static void quit(EventLoop *p) {
    DEBUG << "loop: " << p;
    p->quit();
}

void test1() {
    EventLoopThread th;
}

void test2() {
    EventLoopThread th;
    EventLoop *loop = th.start();
    loop->run_in_loop([=] { return print(loop); });
    CurrentThread::sleep(1 * 1000);
}

void test3() {

    EventLoopThread th;
    EventLoop *loop = th.start();
    loop->run_in_loop([=] { return quit(loop); });
    CurrentThread::sleep(1 * 1000);
}

int main(int argc, const char *argv[]) {

//    test1();
//    test2();
    test3();

    return 0;
}