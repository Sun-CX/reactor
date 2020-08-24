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

using std::for_each;
using std::placeholders::_1;

thread_local EventLoop *EventLoop::loop_in_this_thread;
int EventLoop::default_timeout_milliseconds = -1;   // 默认永远等待

EventLoop::EventLoop() : looping(false), exited(false), pid(CurrentThread::pid),
                         poller(Poller::default_poller(this)), thread_name(CurrentThread::name) {
    if (unlikely(loop_in_this_thread != nullptr)) {
        fprintf(stderr, "Another EventLoop already existed in %s[%d].", thread_name, pid);
        exit(0);
    } else loop_in_this_thread = this;
}

EventLoop::~EventLoop() { loop_in_this_thread = nullptr; }

void EventLoop::loop() {
    assert_in_created_thread();
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
    assert_in_created_thread();
    poller->update_channel(channel);
}

void EventLoop::remove_channel(Channel *channel) {
    poller->remove_channel(channel);
}

bool EventLoop::has_channel(Channel *channel) {
    return poller->has_channel(channel);
}

bool EventLoop::is_in_created_thread() const {
    return pid == CurrentThread::pid;
}

void EventLoop::assert_in_created_thread() {
    if (!is_in_created_thread()) ERROR_EXIT("assert_in_created_thread failed.");
}

void EventLoop::quit() {
    exited = true;
    //TODO: 跨线程调用
//    if (!is_in_created_thread()) {
//        wakeup();
//    }
}

void EventLoop::run_in_loop(const function<void()> &func) {
    if (is_in_created_thread()) {
        func();
    } else {
        //TODO: queue_in_loop(cb);
    }
}

EventLoop *EventLoop::event_loop_of_current_thread() {
    return loop_in_this_thread;
}
