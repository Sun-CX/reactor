//
// Created by suncx on 2020/8/21.
//

#include "PollPoller.h"
#include "Timestamp.h"
#include "Exception.h"
#include "Channel.h"
#include <poll.h>
#include <cassert>

using std::iter_swap;

PollPoller::PollPoller(EventLoop *loop) : Poller(loop) {}

Timestamp PollPoller::poll(Poller::Channels *active_channels, int milliseconds) {
    auto num_events = ::poll(fds.data(), fds.size(), milliseconds);
    Timestamp now = Timestamp::now();
    int saved_no = errno;
    if (unlikely(num_events == -1)) {
        if (saved_no != EINTR) {
            errno = saved_no;
            fprintf(stderr, "PollPoller::poll() error.\n");
        }
    } else if (num_events == 0) {   // 无活跃的文件描述符，超时返回
        printf("poll timeout, nothing happened.\n");
    } else {
//        printf("poll() received %d events...\n", num_events);
        fill_active_channels(active_channels, num_events);
    }
    return now;
}

void PollPoller::update_channel(Channel *channel) {
#ifndef NDEBUG
    Poller::assert_in_loop_thread();
#endif
    auto idx = channel->get_index();
    if (idx < 0) { // a new one
        assert(channel_map.find(channel->get_fd()) == channel_map.cend());
        pollfd pfd;
        pfd.fd = channel->get_fd();
        pfd.events = channel->get_events();
        pfd.revents = 0;
        fds.push_back(pfd);
        channel->set_index(static_cast<int>(fds.size() - 1));
        channel_map[pfd.fd] = channel;
    } else {    // update existed one
        assert(channel_map.find(channel->get_fd()) != channel_map.cend());
        assert(channel_map[channel->get_fd()] == channel);
        assert(0 <= idx and idx < fds.size());

        pollfd &pfd = fds[idx];
        pfd.fd = channel->get_fd();
        pfd.events = static_cast<short>(channel->get_events());
        pfd.revents = 0;
        /**
         * The field fd contains a file descriptor for an open file. If this field is negative, then the corresponding events field is ignored and the revents field returns zero.
         * This provides an easy way of ignoring a file descriptor for a single poll() call: simply negate the fd field.
         * Note, however, that this technique can't be used to ignore file descriptor 0.
         * 如果某一个 channel 暂时不关心任何事件，那么可以把 pollfd.fd 设置为负数，这样 poll 会忽略此文件描述符
         * 不能仅仅将 pollfd.events 设置为 0，因为无法屏蔽 POLLERR 事件
         * pfd.fd 进行减一操作是为了解决 fd 可能为 0 的情况：因为 -0 == 0，所以要减一
         */
        if (channel->has_none_events()) pfd.fd = -channel->get_fd() - 1;
    }
}

void PollPoller::remove_channel(Channel *channel) {
#ifndef NDEBUG
    Poller::assert_in_loop_thread();
#endif
    auto idx = channel->get_index();
    pollfd &pfd = fds[idx];
    auto n = channel_map.erase(channel->get_fd());
    assert(n == 1);
    if (idx == fds.size() - 1) {
        fds.pop_back();
    } else {
        //将删除的元素与末尾元素交换，避免元素无谓地移动
        auto channel_at_end = fds.back().fd;
        iter_swap(fds.begin() + idx, fds.end() - 1);

        if (channel_at_end < 0) {
            channel_at_end = -channel_at_end - 1;
        }
        channel_map[channel_at_end]->set_index(idx);
        fds.pop_back();
    }
}

void PollPoller::fill_active_channels(Poller::Channels *active_channels, int num_events) const {
    for (auto it = fds.cbegin(); it != fds.cend() and num_events > 0; ++it) {
        if (it->revents > 0) {
            num_events--;
            auto v = channel_map.find(it->fd);
            auto channel = v->second;
            channel->set_revents(it->revents);
            active_channels->push_back(channel);
        }
    }
}
