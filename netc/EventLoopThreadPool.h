//
// Created by suncx on 2020/8/20.
//

#ifndef REACTOR_EVENTLOOPTHREADPOOL_H
#define REACTOR_EVENTLOOPTHREADPOOL_H

#include "NonCopyable.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

using std::string;
using std::vector;
using std::function;
using std::unique_ptr;

class EventLoop;

class EventLoopThread;

class EventLoopThreadPool final : public NonCopyable {
private:
    using ThreadInitCallback = function<void(EventLoop *)>;

    EventLoop *loop;
    string name;
    bool started;
    int num_threads;
    int next;
    vector<unique_ptr<EventLoopThread>> threads;
    vector<EventLoop *> loops;

public:
    EventLoopThreadPool(EventLoop *base_loop, string name);

    void set_thread_num(int num);

    void start(const ThreadInitCallback &callback);

    EventLoop *get_next_loop();
};

#endif //REACTOR_EVENTLOOPTHREADPOOL_H
