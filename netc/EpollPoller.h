//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EPOLLPOLLER_H
#define REACTOR_EPOLLPOLLER_H

#include "Poller.h"

/*
 * typedef union epoll_data {
 *     void *ptr;
 *     int fd;
 *     uint32_t u32;
 *     uint64_t u64;
 * } epoll_data_t;
 *
 * struct epoll_event {
 *     uint32_t events;
 *     epoll_data_t data;
 * };
 */

class epoll_event;

namespace reactor::net {
    using std::chrono::system_clock;

    class EpollPoller final : public Poller {
    private:
        using EpollEvents = vector<epoll_event>;

        // a new channel, not added in epoll management and put into `channel_map`.
        static const int NEW;

        // a channel added in epoll management and has been put into `channel_map`.
        static const int ADD;

        const int epoll_fd;
        EpollEvents events;

        void fill_active_channels(Channels &active_channels, int num_events) const;

        void update(Channel *channel, int operation);

        [[nodiscard]]
        int epoll_open() const;

        void epoll_close(int fd) const;

    public:
        explicit EpollPoller(EventLoop *loop);

        ~EpollPoller() override;

        Timestamp poll(Channels &active_channels, int milliseconds) override;

        // update watched events on channel.
        void update_channel(Channel *channel) override;

        // remove channel from epoll mechanism.
        void remove_channel(Channel *channel) override;
    };
}

#endif //REACTOR_EPOLLPOLLER_H
