//
// Created by suncx on 2020/8/20.
//

#include "EventLoopThread.h"
#include "EventLoop.h"
#include "ConsoleStream.h"

using std::bind;

EventLoopThread::EventLoopThread(ThreadInitializer initializer, string name) :
        loop(nullptr),
        thread(bind(&EventLoopThread::thread_func, this), move(name)),
        mutex(),
        condition(mutex),
        initial(move(initializer)) {}

EventLoopThread::~EventLoopThread() {
    INFO << "---------------------- ~EventLoopThread ----------------------";
    if (loop != nullptr) {
        loop->quit();
        thread.join();
    }
}

void EventLoopThread::thread_func() {
    EventLoop lo;
    if (initial) initial(&lo);
    {
        MutexGuard guard(mutex);
        loop = &lo;
        condition.notify();
    }
    loop->loop();
    {
        MutexGuard guard(mutex);
        loop = nullptr;
    }
}

EventLoop *EventLoopThread::start() {
    thread.start();
    EventLoop *lo = nullptr;
    {
        MutexGuard guard(mutex);
        while (loop == nullptr) {
            condition.wait();
        }
        lo = loop;
    }
    return lo;
}