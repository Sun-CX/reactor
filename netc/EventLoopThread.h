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

    using ThreadInitializer = function<void(EventLoop *)>;

    EventLoop *loop;
    Thread thread;
    Mutex mutex;
    Condition condition;
    ThreadInitializer initial; // 在 EventLoop::loop() 之前被调用

    void thread_routine();

public:
    explicit EventLoopThread(ThreadInitializer callback = nullptr, string name = "");

    ~EventLoopThread();

    EventLoop *start();
};

#endif //REACTOR_EVENTLOOPTHREAD_H
