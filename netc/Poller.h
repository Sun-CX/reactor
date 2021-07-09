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
    using std::map;
    using std::vector;
    using reactor::core::NonCopyable;
    using reactor::core::Timestamp;

    class EventLoop;

    class Channel;

    // wrapper class for poll or epoll system call.
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

        // 移除 channel，在调用此方法之前，都会先调用 channel->disable_all();
        virtual void remove_channel(Channel *channel) = 0;

        virtual bool has_channel(Channel *channel) const;

        static Poller *default_poller(EventLoop *loop);
    };
}

#endif //REACTOR_POLLER_H
