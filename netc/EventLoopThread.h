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
    using ThreadInitialCallback = function<void(EventLoop *)>;

    EventLoop *loop;
    bool exiting;
    Thread thread;
    Mutex mutex;
    Condition condition;

    void thread_func();

    ThreadInitialCallback initial_callback; // 在 EventLoop::loop() 函数之前被调用
public:
    explicit EventLoopThread(ThreadInitialCallback callback = ThreadInitialCallback(), string name = "");

    virtual ~EventLoopThread();

    EventLoop *start();
};


#endif //REACTOR_EVENTLOOPTHREAD_H
