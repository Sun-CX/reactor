//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_POLLER_H
#define REACTOR_POLLER_H

#include "NonCopyable.h"
#include <vector>
#include <map>

using std::map;
using std::vector;

class Timestamp;

class EventLoop;

class Channel;

/**
 * 轮询器：具体实现可以是 poll 或者 epoll
 */
class Poller : public NonCopyable {
private:
    EventLoop *loop;
protected:
    using Channels = vector<Channel *>;
    using ChannelMap = map<int, Channel *>;
    ChannelMap channel_map;

    void assert_in_loop_thread() const;

public:
    explicit Poller(EventLoop *loop);

    virtual ~Poller() = default;

    /**
     * 轮询活跃的事件
     * @param active_channels
     * @param milliseconds 超时时间（毫秒），值为负数为永不超时
     * @return
     */
    virtual Timestamp poll(Channels &active_channels, int milliseconds) = 0;

    virtual void update_channel(Channel *channel) = 0;

    // 移除通道，在调用此方法之前，通常都会先调用 Channel->disable_all();
    virtual void remove_channel(Channel *channel) = 0;

    virtual bool has_channel(Channel *channel) const;

    static Poller *default_poller(EventLoop *loop);
};

#endif //REACTOR_POLLER_H
