//
// Created by suncx on 2020/8/20.
//

#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"

using std::make_unique;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *base_loop, string name) :
        loop(base_loop), name(move(name)), started(false), num_threads(0), next(0) {}

void EventLoopThreadPool::set_thread_num(int num) {
    num_threads = num;
}

void EventLoopThreadPool::start(const EventLoopThreadPool::ThreadInitCallback &callback) {
    loop->assert_in_created_thread();
    started = true;
    char buf[name.size() + 32];
    for (int i = 0; i < num_threads; ++i) {
        snprintf(buf, sizeof(buf), "%s-%d", name.c_str(), i + 1);
        auto th = new EventLoopThread(callback, string(buf));
        threads.push_back(unique_ptr<EventLoopThread>(th));
        loops.push_back(th->start());
    }
    if (num_threads == 0 and callback) {
        callback(loop);
    }
}

EventLoop *EventLoopThreadPool::get_next_loop() {
    loop->assert_in_created_thread();
    EventLoop *lo = loop;
    if (!loops.empty()) {
        loop = loops[next++];
        if (next == loops.size()) next = 0;
    }
    return lo;
}

