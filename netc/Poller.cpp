//
// Created by suncx on 2020/8/17.
//

#include "Poller.h"
#include "Exception.h"
#include "Channel.h"
#include "EventLoop.h"
#include "PollPoller.h"
#include <cassert>

Poller::Poller(EventLoop *loop) : loop(loop) {}

void Poller::assert_in_loop_thread() const {
    if (unlikely(!loop->is_in_loop_thread())) ERROR_EXIT("assert_in_loop_thread failed.");
}

bool Poller::has_channel(Channel *channel) const {
    assert(loop->is_in_loop_thread());
    auto it = channel_map.find(channel->get_fd());
    return it != channel_map.end() and it->second == channel;
}

Poller *Poller::default_poller(EventLoop *loop) {
    return new PollPoller(loop);
}

