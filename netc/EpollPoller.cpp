//
// Created by suncx on 2020/8/17.
//

#include "EpollPoller.h"
#include "Exception.h"
#include "Channel.h"
#include "Timestamp.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

const int EpollPoller::NEW = -1;
const int EpollPoller::ADD = 1;
const int EpollPoller::DEL = 2;

const char *EpollPoller::operation_to_string(int op) {
    switch (op) {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_MOD:
            return "MOD";
        case EPOLL_CTL_DEL:
            return "DEL";
        default:
            return "BAD OPERATION";
    }
}

EpollPoller::EpollPoller(EventLoop *loop) : Poller(loop), epoll_fd(epoll_create1(EPOLL_CLOEXEC)), events(16) {
    if (unlikely(epoll_fd < 0)) ERROR_EXIT("create epoll_fd error.");
}

EpollPoller::~EpollPoller() {
    auto status = close(epoll_fd);
    if (unlikely(status < 0)) fprintf(stderr, "close epoll_fd %d error with errno: %d.\n", epoll_fd, errno);
}

void EpollPoller::fill_active_channels(Poller::Channels *active_channels, int num_events) const {
    for (int i = 0; i < num_events; ++i) {
        auto channel = static_cast<Channel *>(events[i].data.ptr);
        channel->set_revents(events[i].events);
        active_channels->push_back(channel);
    }
}

void EpollPoller::update(Channel *channel, int operation) {
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = channel->get_events();
    ev.data.ptr = channel;
    auto fd = channel->get_fd();

    if (epoll_ctl(epoll_fd, operation, fd, &ev) < 0) {
        fprintf(stderr, "epoll_ctl op: %s, fd: %d\n", operation_to_string(operation), fd);
    }
}

Timestamp EpollPoller::poll(Poller::Channels *active_channels, int milliseconds) {
    auto num_events = epoll_wait(epoll_fd, events.data(), events.size(), milliseconds);
    auto now = Timestamp::now();

    if (unlikely(num_events < 0)) {
        fprintf(stderr, "%s invoked error, errno: %d.\n", __PRETTY_FUNCTION__, errno);
    } else if (num_events == 0) {
        printf("epoll_wait timeout, nothing happened.\n");
    } else {
        fill_active_channels(active_channels, num_events);
        if (num_events == events.size()) events.resize(events.size() * 2);
    }
    return now;
}

void EpollPoller::update_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    auto idx = channel->get_index();
    int fd = channel->get_fd();
    if (idx == NEW or idx == DEL) {
        if (idx == NEW) channel_map[fd] = channel;
        channel->set_index(ADD);
        update(channel, EPOLL_CTL_ADD);
    } else {
        if (channel->none_events_watched()) {
            update(channel, EPOLL_CTL_DEL);
            channel->set_index(DEL);
        } else {
            update(channel, EPOLL_CTL_MOD);
        }
    }
}

void EpollPoller::remove_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    int fd = channel->get_fd();
    int index = channel->get_index();
    size_t n = channel_map.erase(fd);
    if (n != 1) fprintf(stderr, "channel_map has deleted more than one Channel * ?");
    if (index == ADD) {
        update(channel, EPOLL_CTL_DEL);
    }
    channel->set_index(NEW);
}