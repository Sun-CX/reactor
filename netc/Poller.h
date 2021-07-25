//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_POLLER_H
#define REACTOR_POLLER_H

#include "NonCopyable.h"
#include "Timestamp.h"
#include <vector>
#include <map>

namespace reactor::net {
    using reactor::core::NonCopyable;
    using reactor::core::Timestamp;
    using std::map;
    using std::vector;

    class EventLoop;

    class Channel;

    // abstract class for poll or epoll system call.
    class Poller : public NonCopyable {
    private:
        EventLoop *const loop;
    protected:
        using Channels = vector<Channel *>;
        using ChannelMap = map<int, Channel *>;
        ChannelMap channel_map;

        void assert_in_loop_thread() const;

    public:
        explicit Poller(EventLoop *loop);

        virtual ~Poller() = default;

        virtual Timestamp poll(Channels &active_channels, int milliseconds) = 0;

        virtual void update_channel(Channel *channel) = 0;

        virtual void remove_channel(Channel *channel) = 0;

        virtual bool has_channel(Channel *channel) const;

        static Poller *default_poller(EventLoop *loop);
    };
}

#endif //REACTOR_POLLER_H
