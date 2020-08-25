//
// Created by suncx on 2020/8/21.
//

#ifndef REACTOR_POLLPOLLER_H
#define REACTOR_POLLPOLLER_H

#include "Poller.h"

/**
 * Data structure describing a polling request.
 * struct pollfd {
 *      int fd;			//File descriptor to poll.
 *      short int events;		//Types of events poller cares about.
 *      short int revents;		//Types of events that actually occurred.
 * }
 */
struct pollfd;

class PollPoller final : public Poller {
private:
    using PollFDs = vector<pollfd>;

    PollFDs fds;

    void fill_active_channels(Channels *active_channels, int num_events) const;

public:
    explicit PollPoller(EventLoop *loop);

    void update_channel(Channel *channel) override;

    void remove_channel(Channel *channel) override;

    Timestamp poll(Channels *active_channels, int milliseconds) override;
};

#endif //REACTOR_POLLPOLLER_H
