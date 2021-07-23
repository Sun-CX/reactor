//
// Created by suncx on 2020/8/17.
//

#include "EpollPoller.h"
#include "Channel.h"
#include "Ext.h"
#include "ConsoleStream.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

using std::chrono::system_clock;
using reactor::net::EpollPoller;

const int EpollPoller::NEW = -1;
const int EpollPoller::ADD = 0;

EpollPoller::EpollPoller(EventLoop *loop) :
        Poller(loop),
        epoll_fd(epoll_open()),
        events() {
    events.reserve(16);
}

EpollPoller::~EpollPoller() {
    epoll_close(epoll_fd);
}

system_clock::time_point EpollPoller::poll(Channels &active_channels, int milliseconds) {
    int ready_events = ::epoll_wait(epoll_fd, events.data(), events.capacity(), milliseconds);
    system_clock::time_point now = system_clock::now();
    if (unlikely(ready_events < 0)) {
        if (errno != EINTR)
            RC_FATAL << "epoll(" << epoll_fd << ") wait error: " << ::strerror(errno);
    } else if (ready_events == 0) {
        RC_WARN << "epoll(" << epoll_fd << ") timeout, nothing happened";
    } else {
        fill_active_channels(active_channels, ready_events);

        // FIXME: optimize enlarge capacity strategy.
        if (ready_events == events.capacity())
            events.reserve(events.capacity() << 1u);
    }
    return now;
}

void EpollPoller::fill_active_channels(Channels &active_channels, int num_events) const {
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
    if (unlikely(::epoll_ctl(epoll_fd, operation, fd, &evt) < 0))
        RC_FATAL << "epoll(" << epoll_fd << ") ctl error: " << ::strerror(errno);
}

void EpollPoller::update_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    int idx = channel->get_index();
    int fd = channel->get_fd();

    switch (idx) {
        case NEW:
            assert(channel_map.find(fd) == channel_map.cend());
            channel_map[fd] = channel;
            update(channel, EPOLL_CTL_ADD);
            channel->set_index(ADD);

            break;
        case ADD:
            assert(channel_map.find(fd) != channel_map.cend());
            update(channel, EPOLL_CTL_MOD);

            break;
        default:
            RC_FATAL << "invalid channel state";
    }
}

void EpollPoller::remove_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    assert(channel->get_index() == ADD);

    update(channel, EPOLL_CTL_DEL);

    int fd = channel->get_fd();
    auto n = channel_map.erase(fd);
    assert(n == 1);
    channel->set_index(NEW);
}

int EpollPoller::epoll_open() const {
    int fd;
    if (unlikely((fd = ::epoll_create1(EPOLL_CLOEXEC)) < 0))
        RC_FATAL << "epoll create error: " << ::strerror(errno);

    return fd;
}

void EpollPoller::epoll_close(int fd) const {
    if (unlikely(::close(fd) < 0))
        RC_FATAL << "close epoll(" << fd << ") error: " << ::strerror(errno);
}
