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

thread_local EventLoop *EventLoop::loop_in_this_thread;

EventLoop::EventLoop() : looping(false), is_quit(false), pid(CurrentThread::pid), poller(Poller::default_poller(this)),
                         thread_name(CurrentThread::name) {

//    printf("poller == null: %d\n", poller == nullptr);

    if (unlikely(loop_in_this_thread != nullptr)) {
        fprintf(stderr, "Another EventLoop already existed in %s[%d].", thread_name, pid);
        exit(0);
    } else loop_in_this_thread = this;
}

EventLoop::~EventLoop() { loop_in_this_thread = nullptr; }

void EventLoop::loop() {
    assert_in_created_thread();
    looping = true;
    is_quit = false;

    printf("EventLoop %p start loop...\n", this);

    while (!is_quit) {
        active_channels.clear();
        poller->poll(&active_channels, 1000);

        for_each(active_channels.cbegin(), active_channels.cend(), [](const Channel *channel) {
//            channel->handle_event();
        });

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
    is_quit = true;
    if (!is_in_created_thread()) {
//        wakeup();
    }
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

//TimerId EventLoop::run_at(const Timer::TimerCallback &callback, Timestamp timestamp) {
//
//
//    return ;
//}
