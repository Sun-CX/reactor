//
// Created by suncx on 2020/9/5.
//

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "TimerId.h"

using std::bind;

static void run() {
    printf("=================== pid: %d, thread: %s[%d] ===================\n", getpid(), CurrentThread::name,
           CurrentThread::pid);
}

static void print(EventLoop *loop = nullptr) {
    printf("pid: %d, tid: %d, loop: %p\n", getpid(), CurrentThread::pid, loop);
}

static void quit(EventLoop *loop) {
    print(loop);
    loop->quit();
}

static void test1() {
    run();
    EventLoopThread th;
    EventLoop *loop = th.start();
    loop->run_in_loop(run);

    printf("now: %s\n", Timestamp::now().to_fmt_string().c_str());
    loop->run_after(run, 3);

    sleep(4);
}

int main(int argc, const char *argv[]) {

//    print();
//    {
//        EventLoopThread th;
//    }
//
//    {
//        EventLoopThread t1;
//        EventLoop *loop = t1.start();
//        loop->run_in_loop(bind(print, loop));
//    }
//
//    {
//        EventLoopThread t2;
//        EventLoop *loop = t2.start();
//        loop->run_in_loop(bind(quit, loop));
//    }
    test1();
    return 0;
}