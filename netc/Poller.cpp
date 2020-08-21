//
// Created by suncx on 2020/8/17.
//

#include "Poller.h"
#include "Channel.h"
#include "EventLoop.h"
#include "PollPoller.h"

Poller::Poller(EventLoop *loop) : loop(loop) {}

void Poller::assert_in_loop_thread() const {
    loop->assert_in_created_thread();
}

Poller *Poller::default_poller(EventLoop *loop) {
    return new PollPoller(loop);
}

bool Poller::has_channel(Channel *channel) const {
    assert_in_loop_thread();
    auto it = channel_map.find(channel->get_fd());
    return it != channel_map.end() and it->second == channel;
}

