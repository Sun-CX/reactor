//
// Created by suncx on 2020/8/20.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include <cassert>

using std::make_unique;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *base_loop, string name, int num_threads) :
        loop(base_loop),
        prefix(move(name)),
        num_threads(num_threads),
        next(0) {
    assert(num_threads >= 0);
    threads.reserve(num_threads);
    loops.reserve(num_threads);
}

void EventLoopThreadPool::start(const EventLoopThread::ThreadInitializer &callback) {
    assert(loop->is_in_loop_thread());
    char thread_name[prefix.size() + 8];
    for (int i = 0; i < num_threads; ++i) {
        snprintf(thread_name, sizeof(thread_name), "%s-%d", prefix.c_str(), i + 1);
        threads.emplace_back(new EventLoopThread(callback, thread_name));
        loops.push_back(threads.back()->start());
    }
    if (num_threads == 0 and callback) callback(loop);
}

EventLoop *EventLoopThreadPool::get_next_loop() {
    assert(loop->is_in_loop_thread());
    EventLoop *lo = loop;
    if (!loops.empty()) {
        lo = loops[next++];
        if (next == loops.size()) next = 0;
    }
    return lo;
}