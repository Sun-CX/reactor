//
// Created by suncx on 2020/8/20.
//

#ifndef REACTOR_EVENTLOOPTHREAD_H
#define REACTOR_EVENTLOOPTHREAD_H

#include "Thread.h"
#include "Condition.h"

class EventLoop;

class EventLoopThread final : public NonCopyable {
private:
    friend class EventLoopThreadPool;

    friend class TcpServer;

    using ThreadInitialCallback = function<void(EventLoop *)>;

    EventLoop *loop;
    Thread thread;
    Mutex mutex;
    Condition condition;
    ThreadInitialCallback initial_callback; // 在 EventLoop::loop() 之前被调用

    void thread_func();

public:
    explicit EventLoopThread(ThreadInitialCallback callback = nullptr, string name = "");

    ~EventLoopThread();

    EventLoop *start();
};

#endif //REACTOR_EVENTLOOPTHREAD_H
