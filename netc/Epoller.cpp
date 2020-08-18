//
// Created by suncx on 2020/8/17.
//

#include "Epoller.h"

const char *Epoller::operation_to_string(int op) {
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

void Epoller::fill_active_channels(Poller::Channels *active_channels, int num_events) const {
    for (int i = 0; i < num_events; ++i) {
        auto channel = static_cast<Channel *>(events[i].data.ptr);
        channel->set_revents(events[i].events);
        active_channels->emplace_back(channel);
    }
}

void Epoller::update(Channel *channel, int operation) {
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = channel->get_events();
    ev.data.ptr = channel;
    auto fd = channel->get_fd();

    if (epoll_ctl(epoll_fd, operation, fd, &ev) < 0) {
        fprintf(stderr, "epoll_ctl op: %s, fd: %d\n", operation_to_string(operation), fd);
    }
}

Epoller::Epoller(EventLoop *loop) : Poller(loop), epoll_fd(epoll_create1(EPOLL_CLOEXEC)), events(16) {
    if (epoll_fd <= 0) {
        ERROR_EXIT("create epoll_fd error.");
    }
}

Epoller::~Epoller() {
    close(epoll_fd);
}

Timestamp Epoller::poll(Poller::Channels *active_channels, int timeout_micro_seconds) {
    auto num_events = epoll_wait(epoll_fd, &*events.begin(), events.size(), timeout_micro_seconds);
    auto err = errno;
    Timestamp now = Timestamp::now();
    if (num_events > 0) {
        fill_active_channels(active_channels, num_events);
        if (num_events == events.size()) events.resize(events.size() * 2);
    } else if (num_events == 0) {
        printf("Nothing happened.");
    } else {
        if (err == EINTR) ERROR_EXIT("epoll_wait error.");
    }
    return now;
}

void Epoller::update_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    const int index = channel->get_index();
    if (index == -1 or index == 2) {
        int fd = channel->get_fd();
        if (index == -1) {
            channel_map[fd] = channel;
        }
        channel->set_index(1);
        update(channel, EPOLL_CTL_ADD);
    } else {
        int fd = channel->get_fd();
        if (channel->none_events()) {
            update(channel, EPOLL_CTL_DEL);
            channel->set_index(2);
        } else {
            update(channel, EPOLL_CTL_MOD);
        }
    }
}

void Epoller::remove_channel(Channel *channel) {
    Poller::assert_in_loop_thread();
    int fd = channel->get_fd();
    int index = channel->get_index();
    size_t n = channel_map.erase(fd);
    if (index == 1) {
        update(channel, EPOLL_CTL_DEL);
    }
    channel->set_index(-1);
}
