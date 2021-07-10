//
// Created by suncx on 2020/8/20.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include <cassert>

using reactor::net::EventLoopThreadPool;
using reactor::net::EventLoop;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *base_loop, string name, int num_threads) :
        loop(base_loop),
        prefix(move(name)),
        num_threads(num_threads),
        next(0) {
    assert(num_threads >= 0);
    worker_threads.reserve(num_threads);
    worker_loops.reserve(num_threads);
}

void EventLoopThreadPool::start(const EventLoopThread::ThreadInitializer &callback) {
    assert(loop->is_in_created_thread());
    char thread_name[prefix.size() + 8];
    for (int i = 0; i < num_threads; ++i) {
        snprintf(thread_name, sizeof(thread_name), "%s-%d", prefix.c_str(), i + 1);
        worker_threads.emplace_back(new EventLoopThread(callback, thread_name));
        worker_loops.push_back(worker_threads.back()->start());
    }
    if (num_threads == 0 and callback) callback(loop);
}

EventLoop *EventLoopThreadPool::get_next_loop() {
    assert(loop->is_in_created_thread());
    EventLoop *lo = loop;
    if (!worker_loops.empty()) {
        lo = worker_loops[next++];
        if (next == worker_loops.size()) next = 0;
    }
    return lo;
}