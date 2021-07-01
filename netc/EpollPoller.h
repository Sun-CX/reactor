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
    using reactor::core::Timestamp;

    class EpollPoller final : public Poller {
    private:
        using EpollEvents = vector<epoll_event>;

        const int epoll_fd;     // epoll 文件描述符：调用 epoll_create1() 返回
        EpollEvents events;     // 活动的 fd 列表

        /* fd 在 epoll 管理下的三种状态 */
        static const int NEW; // 新增 fd
        static const int ADD; // 已添加
        static const int DEL; // 已脱离 epoll 管理，但未从 channel_map 中清除

        void fill_active_channels(Channels &active_channels, int num_events) const;

        void update(Channel *channel, int operation);

    public:
        explicit EpollPoller(EventLoop *loop);

        ~EpollPoller() override;

        Timestamp poll(Channels &active_channels, int milliseconds) override;

        void update_channel(Channel *channel) override;

        void remove_channel(Channel *channel) override;
    };
}

#endif //REACTOR_EPOLLPOLLER_H
