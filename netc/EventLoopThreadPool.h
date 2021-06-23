//
// Created by suncx on 2020/8/20.
//

#ifndef REACTOR_EVENTLOOPTHREADPOOL_H
#define REACTOR_EVENTLOOPTHREADPOOL_H

#include "EventLoopThread.h"
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::function;
using std::unique_ptr;

class EventLoop;

class EventLoopThreadPool final : public NonCopyable {
private:
    EventLoop *loop;
    const string prefix;
    const int num_threads;
    int next;
    vector<unique_ptr<EventLoopThread>> threads;
    vector<EventLoop *> loops;

public:
    EventLoopThreadPool(EventLoop *base_loop, string name, int num_threads);

    void start(const EventLoopThread::ThreadInitializer &callback);

    EventLoop *get_next_loop();
};

#endif //REACTOR_EVENTLOOPTHREADPOOL_H