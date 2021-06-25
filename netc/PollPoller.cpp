//
// Created by suncx on 2020/8/21.
//

#include "PollPoller.h"
#include "Timestamp.h"
#include "GnuExt.h"
#include "Channel.h"
#include "ConsoleStream.h"
#include <poll.h>
#include <cassert>

using std::iter_swap;

PollPoller::PollPoller(EventLoop *loop) : Poller(loop) {}

Timestamp PollPoller::poll(Channels &active_channels, int milliseconds) {
    auto num_events = ::poll(fds.data(), fds.size(), milliseconds);
    auto now = Timestamp::now();
    if (unlikely(num_events < 0)) { // error
        WARN << "poll error, errno = " << errno;
    } else if (num_events == 0) {   // timeout
        INFO << "poll timeout...";
    } else {
        fill_active_channels(active_channels, num_events);
    }
    return now;
}

void PollPoller::fill_active_channels(Poller::Channels &active_channels, int num_events) const {
    for (auto it = fds.cbegin(); it != fds.cend() and num_events > 0; ++it) {
        if (it->revents > 0) {
            num_events--;
            auto v = channel_map.find(it->fd);
            auto channel = v->second;
            channel->set_revents(it->revents);
            active_channels.push_back(channel);
        }
    }
}

void PollPoller::update_channel(Channel *channel) {
#ifndef NDEBUG
    Poller::assert_in_loop_thread();
#endif
    auto idx = channel->get_index();
    if (idx < 0) {  // a new one
        assert(channel_map.find(channel->get_fd()) == channel_map.cend());
        assert(!channel->no_events_watched());
        pollfd pfd;
        pfd.fd = channel->get_fd();
        pfd.events = channel->get_events();
        pfd.revents = 0;
        fds.push_back(pfd);
        channel->set_index(static_cast<int>(fds.size() - 1));
        channel_map[channel->get_fd()] = channel;
    } else {    // update existed one, usually disable all events.
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
         *
         * 标准输入 0    stdin 从键盘获得输入
         * 标准输出 1    stdout 输出到控制台
         * 错误输出 2    stderr 输出到控制台
         *
         * 如果某一个 channel 暂时不关心任何事件，那么可以把 pollfd.fd 设置为负数，这样 poll 会忽略此文件描述符
         * 不能仅仅将 pollfd.events 设置为 0，因为无法屏蔽 POLLHUP, POLLERR and POLLNVAL 事件
         * pfd.fd 取相反数再减一的操作是为了解决 fd 可能为 0 的问题：因为 -0 = 0，因此要减一
         */
        if (channel->no_events_watched())
            pfd.fd = -channel->get_fd() - 1;
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
    if (idx == fds.size() - 1) fds.pop_back();
    else {
        //将删除的元素与末尾元素交换，避免元素无谓地移动
        auto end_channel_fd = fds.back().fd;
        iter_swap(fds.begin() + idx, fds.end() - 1);
        fds.pop_back();

        if (end_channel_fd < 0)
            end_channel_fd = -end_channel_fd - 1;
        channel_map[end_channel_fd]->set_index(idx);
    }
}