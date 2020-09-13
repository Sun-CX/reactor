//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EPOLLPOLLER_H
#define REACTOR_EPOLLPOLLER_H

#include "Poller.h"

class epoll_event;

class EpollPoller final : public Poller {
private:
    using EpollEvents = vector<epoll_event>;

    const int epoll_fd;   // epoll 文件描述符：调用 epoll_create1() 返回
    EpollEvents events;  // 每次 epoll_wait() 调用返回的活动 fd 列表

    void fill_active_channels(Channels *active_channels, int num_events) const;

    void update(Channel *channel, int operation);

    static const int NEW;
    static const int ADD;
    static const int DEL;

    static const char *operation_to_string(int op);

public:
    explicit EpollPoller(EventLoop *loop);

    ~EpollPoller() override;

    Timestamp poll(Channels *active_channels, int milliseconds) override;

    void update_channel(Channel *channel) override;

    void remove_channel(Channel *channel) override;
};

#endif //REACTOR_EPOLLPOLLER_H
