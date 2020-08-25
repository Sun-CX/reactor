//
// Created by suncx on 2020/8/17.
//

#include "EventLoop.h"
#include "Exception.h"
#include "Channel.h"
#include "Timestamp.h"
#include "Poller.h"
#include "CurrentThread.h"
#include <algorithm>
#include <cassert>

using std::for_each;
using std::placeholders::_1;

thread_local EventLoop *EventLoop::loop_in_this_thread;
int EventLoop::default_timeout_milliseconds = -1;   // 默认永不超时

EventLoop::EventLoop() : looping(false), exited(false), pid(CurrentThread::pid),
                         poller(Poller::default_poller(this)) {
    if (unlikely(loop_in_this_thread != nullptr)) {
        fprintf(stderr, "Another EventLoop already existed in %s[%d].", CurrentThread::name, pid);
        exit(0);
    } else loop_in_this_thread = this;
}

EventLoop::~EventLoop() { loop_in_this_thread = nullptr; }

void EventLoop::loop() {
    assert(is_in_loop_thread());

    looping = true;

    printf("EventLoop %p start loop...\n", this);

    while (!exited) {
        active_channels.clear();
        poller->poll(&active_channels, default_timeout_milliseconds);
        for_each(active_channels.cbegin(), active_channels.cend(), bind(&Channel::handle_event, _1));
    }
    looping = false;
}

void EventLoop::update_channel(Channel *channel) {
    assert(is_in_loop_thread() and channel->loop_owner() == this);
    poller->update_channel(channel);
}

void EventLoop::remove_channel(Channel *channel) {
    assert(is_in_loop_thread() and channel->loop_owner() == this);
    poller->remove_channel(channel);
}

bool EventLoop::has_channel(Channel *channel) {
    assert(is_in_loop_thread() and channel->loop_owner() == this);
    return poller->has_channel(channel);
}

bool EventLoop::is_in_loop_thread() const {
    return pid == CurrentThread::pid;
}

void EventLoop::quit() {
    exited = true;
    //TODO: 跨线程调用
//    if (!is_in_created_thread()) {
//        wakeup();
//    }
}

void EventLoop::run_in_loop(const Functor &func) {
    if (is_in_loop_thread()) {
        func();
    } else {
        //TODO: queue_in_loop(cb);
    }
}

EventLoop *EventLoop::event_loop_of_current_thread() {
    return loop_in_this_thread;
}
