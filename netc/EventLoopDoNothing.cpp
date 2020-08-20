//
// Created by suncx on 2020/8/18.
//

#include "EventLoopDoNothing.h"

thread_local bool EventLoopDoNothing::already_existed_in_this_thread;

EventLoopDoNothing::EventLoopDoNothing() : looping(false), thread_name(CurrentThread::name), pid(CurrentThread::pid) {
    printf("EventLoopDoNothing created in %s[%d]\n", CurrentThread::name, pid);
    if (unlikely(already_existed_in_this_thread)) {
        fprintf(stderr, "Another EventLoopDoNothing existed in %s[%d].", thread_name, pid);
        exit(0);
    } else already_existed_in_this_thread = true;
}

EventLoopDoNothing::~EventLoopDoNothing() {
    already_existed_in_this_thread = false;
}

void EventLoopDoNothing::loop() {
    if (is_in_created_thread()) {
        looping = true;
        sleep(3);
        printf("loop finished in %s[%d]\n", CurrentThread::name, CurrentThread::pid);
        looping = false;
    } else {
        fprintf(stderr, "EventLoop can't invoked in another thread.\n");
        exit(0);
    }
}

bool EventLoopDoNothing::is_in_created_thread() const {
    return pid == CurrentThread::pid;
}
