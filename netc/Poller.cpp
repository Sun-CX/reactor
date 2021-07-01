//
// Created by suncx on 2020/8/17.
//

#include "Poller.h"
#include "GnuExt.h"
#include "Channel.h"
#include "EventLoop.h"
#include "PollPoller.h"
#include "EpollPoller.h"
#include "ConsoleStream.h"
#include <cassert>
#include <cstdlib>

using reactor::net::Poller;

Poller::Poller(EventLoop *loop) : loop(loop) {}

void Poller::assert_in_loop_thread() const {
    if (unlikely(!loop->is_in_loop_thread())) FATAL << "assert_in_loop_thread failed.";
}

bool Poller::has_channel(Channel *channel) const {
    assert(loop->is_in_loop_thread());
    auto it = channel_map.find(channel->get_fd());
    return it != channel_map.end() and it->second == channel;
}

Poller *Poller::default_poller(EventLoop *loop) {
//    if (getenv("REACTOR_USE_EPOLL") != nullptr) {
//        return new EpollPoller(loop);
//    } else {
//        return new PollPoller(loop);
//    }
    return new EpollPoller(loop);
}