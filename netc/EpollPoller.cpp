//
// Created by suncx on 2020/8/17.
//

#include "EpollPoller.h"
#include "Exception.h"
#include "Channel.h"
#include "Timestamp.h"
#include "GnuExt.h"
#include "ConsoleStream.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cassert>

using reactor::net::EpollPoller;
using reactor::core::Timestamp;

const int EpollPoller::NEW = -1;
const int EpollPoller::ADD = 0;
const int EpollPoller::DEL = 1;

EpollPoller::EpollPoller(EventLoop *loop) :
        Poller(loop),
        epoll_fd(epoll_create1(EPOLL_CLOEXEC)) {
    if (unlikely(epoll_fd < 0)) RC_FATAL << "create epoll_fd error.";
    events.reserve(16);
    RC_INFO << "create epoll_fd " << epoll_fd;
}

EpollPoller::~EpollPoller() {
    auto status = ::close(epoll_fd);
    if (unlikely(status < 0))
        RC_FATAL << "close epoll_fd " << epoll_fd << " with errno: " << errno;
}

Timestamp EpollPoller::poll(Poller::Channels &active_channels, int milliseconds) {
    auto ready_events = epoll_wait(epoll_fd, events.data(), events.capacity(), milliseconds);
    auto now = Timestamp::now();
    if (unlikely(ready_events < 0)) {
        if (errno != EINTR)
            RC_ERROR << __PRETTY_FUNCTION__ << "invoked error, errno = " << errno;
    } else if (ready_events == 0) {
        RC_WARN << "epoll_wait timeout, nothing happened.";
    } else {
        fill_active_channels(active_channels, ready_events);
        if (ready_events == events.capacity())
            events.reserve(events.capacity() << 1u);
    }
    return now;
}

void EpollPoller::fill_active_channels(Poller::Channels &active_channels, int num_events) const {
    for (int i = 0; i < num_events; ++i) {
        auto channel = static_cast<Channel *>(events[i].data.ptr);
        channel->set_revents(events[i].events);
        active_channels.push_back(channel);
    }
}

void EpollPoller::update(Channel *channel, int operation) {
    epoll_event evt;
    evt.events = channel->get_events();
    evt.data.ptr = channel;
    int fd = channel->get_fd();
    if (epoll_ctl(epoll_fd, operation, fd, &evt) < 0)
        RC_FATAL << "epoll_ctl " << operation << " error.";
}

void EpollPoller::update_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    int idx = channel->get_index();
    int fd = channel->get_fd();
    if (idx == NEW or idx == DEL) {
        if (idx == NEW) channel_map[fd] = channel;
        channel->set_index(ADD);
        update(channel, EPOLL_CTL_ADD);
    } else {
        if (channel->no_events_watched()) {
            update(channel, EPOLL_CTL_DEL);
            channel->set_index(DEL);
        } else update(channel, EPOLL_CTL_MOD);
    }
}

void EpollPoller::remove_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    assert(channel->get_index() == DEL);
    int fd = channel->get_fd();
    auto n = channel_map.erase(fd);
    assert(n == 1);
    channel->set_index(NEW);
}