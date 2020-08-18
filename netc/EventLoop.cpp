//
// Created by suncx on 2020/8/17.
//

#include "EventLoop.h"

thread_local bool EventLoop::already_existed_in_this_thread;

EventLoop::EventLoop() : looping(false), thread_name(CurrentThread::name), pid(CurrentThread::pid) {
    if (unlikely(already_existed_in_this_thread)) {
        fprintf(stderr, "Another EventLoop already existed in %s[%d].", thread_name, pid);
        exit(0);
    } else already_existed_in_this_thread = true;
}

EventLoop::~EventLoop() {
    already_existed_in_this_thread = false;
}

void EventLoop::loop() {
    assert_in_created_thread();
    looping = true;
//    quit = false;
//
//    while (!quit) {
//        ac
//    }

//    poll(nullptr, 0, 5 * 1000);
    looping = false;
}

void EventLoop::update_channel(Channel *channel) {
    assert_in_created_thread();


}

void EventLoop::remove_channel(Channel *channel) {
//    poller->remove_channel(channel);
}

bool EventLoop::has_channel(Channel *channel) {
//    return poller->has_channel(channel);
}

bool EventLoop::is_in_created_thread() const {
    return pid == CurrentThread::pid;
}

void EventLoop::assert_in_created_thread() {
    if (!is_in_created_thread()) ERROR_EXIT("assert_in_created_thread failed.");
}
