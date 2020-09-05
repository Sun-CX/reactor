//
// Created by suncx on 2020/9/5.
//

#include "EventLoop.h"
#include "EventLoopThread.h"

using std::bind;

static void print(EventLoop *loop = nullptr) {
    printf("pid: %d, tid: %d, loop: %p\n", getpid(), CurrentThread::pid, loop);
}

static void quit(EventLoop *loop) {
    print(loop);
    loop->quit();
}

int main(int argc, const char *argv[]) {

    print();
    {
        EventLoopThread th;
    }

    {
        EventLoopThread t1;
        EventLoop *loop = t1.start();
        loop->run_in_loop(bind(print, loop));
    }

    {
        EventLoopThread t2;
        EventLoop *loop = t2.start();
        loop->run_in_loop(bind(quit, loop));
    }
    return 0;
}