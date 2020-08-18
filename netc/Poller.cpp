//
// Created by suncx on 2020/8/17.
//

#include "Poller.h"

Poller::Poller(EventLoop *loop) : loop(loop) {}

void Poller::assert_in_loop_thread() const {
    loop->assert_in_loop_thread();
}

Poller *Poller::default_poller(EventLoop *loop) {
    return reinterpret_cast<Poller *>(new Epoller(loop));
}

