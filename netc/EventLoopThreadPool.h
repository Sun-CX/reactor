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
    using ThreadInitialCallback = function<void(EventLoop *)>;

    EventLoop *loop;
    const string name;
//    bool started;
    const int num_threads;
    int next;
    vector<unique_ptr<EventLoopThread>> threads;
    vector<EventLoop *> loops;

public:
    EventLoopThreadPool(EventLoop *base_loop, int num_threads, string name);

//    void set_thread_num(int num);

    void start(const ThreadInitialCallback &callback);

    EventLoop *get_next_loop();
};

#endif //REACTOR_EVENTLOOPTHREADPOOL_H
