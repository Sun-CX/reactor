//
// Created by suncx on 2020/8/21.
//

#ifndef REACTOR_POLLPOLLER_H
#define REACTOR_POLLPOLLER_H

#include "Poller.h"

struct pollfd;

class PollPoller final : public Poller {
private:
    using PollFDs = vector<pollfd>;

    PollFDs fds;

    void fill_active_channels(Channels *active_channels, int num_events) const;

public:
    explicit PollPoller(EventLoop *loop);

    Timestamp poll(Channels *active_channels, int milliseconds) override;

    void update_channel(Channel *channel) override;

    void remove_channel(Channel *channel) override;
};


#endif //REACTOR_POLLPOLLER_H
