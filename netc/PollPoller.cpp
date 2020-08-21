//
// Created by suncx on 2020/8/21.
//

#include "PollPoller.h"
#include "Timestamp.h"
#include "Exception.h"
#include "Channel.h"
#include <poll.h>

using std::iter_swap;

PollPoller::PollPoller(EventLoop *loop) : Poller(loop) {}

Timestamp PollPoller::poll(Poller::Channels *active_channels, int milliseconds) {
    auto num_events = ::poll(&*fds.begin(), fds.size(), milliseconds);
    Timestamp now = Timestamp::now();
    int saved_no = errno;
    if (unlikely(num_events == -1)) {
        if (saved_no != EINTR) {
            errno = saved_no;
            fprintf(stderr, "PollPoller::poll() error.\n");
        }
    } else if (num_events == 0) {
        printf("poll timeout, nothing happened.\n");
    } else {
        printf("PollPoller::poll() received %d events.\n", num_events);
        fill_active_channels(active_channels, num_events);
    }
    return now;
}

void PollPoller::update_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    if (channel->get_index() < 0) { // a new one
        pollfd pfd;
        pfd.fd = channel->get_fd();
        pfd.events = static_cast<short>(channel->get_events());
        pfd.revents = 0;
        fds.push_back(pfd);
        auto idx = fds.size() - 1;
        channel->set_index(idx);
        channel_map[pfd.fd] = channel;
    } else {
        auto idx = channel->get_index();
        pollfd &pfd = fds[idx];
        pfd.fd = channel->get_fd();
        pfd.events = static_cast<short>(channel->get_events());
        pfd.revents = 0;
        if (channel->has_none_events()) pfd.fd = -channel->get_fd() - 1;
    }
}

void PollPoller::remove_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    auto idx = channel->get_index();
    pollfd &pfd = fds[idx];
    auto n = channel_map.erase(channel->get_fd());

    if (idx == fds.size() - 1) {
        fds.pop_back();
    } else {
        auto channel_at_end = fds.back().fd;
        iter_swap(fds.begin() + idx, fds.end() - 1);

        if (channel_at_end < 0) {
            channel_at_end = -channel_at_end - 1;
        }
        channel_map[channel_at_end]->set_index(idx);
        fds.pop_back();
    }
}

void PollPoller::fill_active_channels(Poller::Channels *active_channels, int num_events) const {
    for (auto it = fds.cbegin(); it != fds.cend() and num_events > 0; ++it) {
        if (it->revents > 0) {
            num_events--;
            auto v = channel_map.find(it->fd);
            auto channel = v->second;
            channel->set_revents(it->revents);
            active_channels->push_back(channel);
        }
    }
}
