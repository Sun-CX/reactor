//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EPOLLER_H
#define REACTOR_EPOLLER_H

#include "Poller.h"
#include <cstring>
#include <sys/epoll.h>

class Epoller : public Poller {
private:
    using Events = vector<epoll_event>;

    int epoll_fd;   // epoll 文件描述符：调用 epoll_create1() 返回
    Events events;  // 每次 epoll_wait() 调用返回的活动 fd 列表

    void fill_active_channels(Channels *active_channels, int num_events) const;

    void update(Channel *channel, int operation);

    static const char *operation_to_string(int op);

public:
    explicit Epoller(EventLoop *loop);

    virtual ~Epoller();

    Timestamp poll(Channels *active_channels, int timeout_micro_seconds) override;

    void update_channel(Channel *channel) override;

    void remove_channel(Channel *channel) override;
};


#endif //REACTOR_EPOLLER_H
