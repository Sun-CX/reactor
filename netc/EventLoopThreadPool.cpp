//
// Created by suncx on 2020/8/20.
//

#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include <cassert>

using std::make_unique;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *base_loop, int num_threads, string name) :
        loop(base_loop), name(move(name)), num_threads(num_threads), next(0) {}

//void EventLoopThreadPool::set_thread_num(int num) {
//    num_threads = num;
//}

void EventLoopThreadPool::start(const ThreadInitialCallback &callback) {
    assert(loop->is_in_loop_thread());
//    started = true;
    char thread_name[name.size() + 8];
    for (int i = 0; i < num_threads; ++i) {
        snprintf(thread_name, sizeof(thread_name), "%s-%d", name.c_str(), i + 1);
        auto th = new EventLoopThread(callback, thread_name);
        threads.emplace_back(th);
        loops.push_back(th->start());
    }
    if (num_threads == 0 and callback) callback(loop);
}

EventLoop *EventLoopThreadPool::get_next_loop() {
    assert(loop->is_in_loop_thread());
    EventLoop *lo = loop;
    if (!loops.empty()) {
        loop = loops[next++];
        if (next == loops.size()) next = 0;
    }
    return lo;
}

