//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_POLLER_H
#define REACTOR_POLLER_H

#include "NonCopyable.h"
#include "EventLoop.h"
#include "Epoller.h"
#include <vector>
#include <map>

using std::map;
using std::vector;

class Poller : public NonCopyable {
private:
    EventLoop *loop;
protected:
    using ChannelMap = map<int, Channel *>;
    ChannelMap channel_map;
public:
    using Channels = vector<Channel *>;

    explicit Poller(EventLoop *loop);

    virtual Timestamp poll(Channels *active_channels, int timeout_micro_seconds) = 0;

    virtual void update_channel(Channel *channel) = 0;

    virtual void remove_channel(Channel *channel) = 0;

    virtual bool has_channel(Channel *channel) = 0;

    void assert_in_loop_thread() const;

    static Poller *default_poller(EventLoop *loop);
};


#endif //REACTOR_POLLER_H
