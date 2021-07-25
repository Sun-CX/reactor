//
// Created by suncx on 2020/8/21.
//

#include "PollPoller.h"
#include "Ext.h"
#include "Channel.h"
#include "ConsoleStream.h"
#include <cstring>
#include <poll.h>
#include <cassert>

using std::iter_swap;
using reactor::core::Timestamp;
using reactor::net::PollPoller;
using std::chrono::system_clock;

PollPoller::PollPoller(EventLoop *loop) : Poller(loop) {}

Timestamp PollPoller::poll(Channels &active_channels, int milliseconds) {

    int ready_events = ::poll(fds.data(), fds.size(), milliseconds);

    Timestamp ts = system_clock::now();

    if (unlikely(ready_events < 0)) { // error
        if (ready_events != EINTR)
            RC_FATAL << "poll wait error: " << ::strerror(errno);
    } else if (ready_events == 0) {   // timeout
        RC_WARN << "poll wait timeout, nothing happened";
    } else {
        fill_active_channels(active_channels, ready_events);
    }
    return ts;
}

void PollPoller::fill_active_channels(Poller::Channels &active_channels, int ready_events) const {

    for (auto it = fds.cbegin(); it != fds.cend() and ready_events > 0; ++it) {

        if (it->revents > 0) {
            auto v = channel_map.find(it->fd);
            auto channel = v->second;
            channel->set_revents(it->revents);
            active_channels.push_back(channel);

            ready_events--;
        }
    }
}

void PollPoller::update_channel(Channel *channel) {
#ifndef NDEBUG
    Poller::assert_in_loop_thread();
#endif
    int index = channel->get_index();

    if (index < 0) { // a new channel

        assert(channel_map.find(channel->get_fd()) == channel_map.cend());

        pollfd pfd;
        pfd.fd = channel->get_fd();
        pfd.events = channel->get_events();
        pfd.revents = 0;

        fds.push_back(pfd);
        channel->set_index(static_cast<int>(fds.size() - 1));

        channel_map[channel->get_fd()] = channel;

    } else { // an existed channel

        assert(channel_map.find(channel->get_fd()) != channel_map.cend());
        assert(channel_map[channel->get_fd()] == channel);
        assert(0 <= index and index < fds.size());

        pollfd &pfd = fds[index];
        assert(pfd.fd == channel->get_fd());
        pfd.events = channel->get_events();
        pfd.revents = 0;
    }
}

void PollPoller::remove_channel(Channel *channel) {
#ifndef NDEBUG
    Poller::assert_in_loop_thread();
#endif
    int index = channel->get_index();

    assert(channel_map.find(channel->get_fd()) != channel_map.cend());
    assert(channel_map[channel->get_fd()] == channel);
    assert(0 <= index and index < fds.size());

    pollfd &pfd = fds[index];

    if (index == fds.size() - 1) // last one
        fds.pop_back();
    else {
        // swap this channel with the last channel.
        int last_channel_fd = fds.back().fd;
        iter_swap(fds.begin() + index, fds.end() - 1);
        fds.pop_back();

        channel_map[last_channel_fd]->set_index(index);
    }
    auto n = channel_map.erase(channel->get_fd());
    assert(n == 1);
}