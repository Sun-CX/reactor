//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include "NonCopyable.h"
#include "EventLoop.h"
#include <memory>
#include <functional>
#include <poll.h>

using std::move;
using std::function;
using std::weak_ptr;
using std::shared_ptr;

class EventLoop;

/**
 * 每个 Channel 对象自始至终只属于一个 EventLoop（即只运行在 IO 线程）
 * 负责某一个文件描述符的 IO 事件分发
 * Channel 作为 epoll_event.data.ptr 的挂载
 */
class Channel final : public NonCopyable {
public:
    using EventCallback  = function<void()>;
    using ReadEventCallback = function<void(Timestamp)>;
private:
    EventLoop *loop;
    const int fd;
    int events;     // 关心的 IO 事件
    int revents;    // 活跃的 IO 事件
    int index;
//    bool loghup;
    weak_ptr<void> tie;
    bool tied;
    bool event_handling;
    bool add_to_loop;

    ReadEventCallback read_callback;
    EventCallback write_callback;
    EventCallback close_callback;
    EventCallback error_callback;

    void handle_event_with_guard(Timestamp timestamp);

    void update();

public:
    Channel(EventLoop *loop, int fd);

//    virtual ~Channel();

    void handle_event(Timestamp timestamp);

    void set_read_callback(ReadEventCallback callback);

    void set_write_callback(EventCallback callback);

    void set_close_callback(EventCallback callback);

    void set_error_callback(EventCallback callback);

    int get_fd() const;

    void set_index(int idx);

    int get_index() const;

    int get_events() const;

    void set_revents(int ev);

    bool none_events() const;

    void remove();

    EventLoop *loop_owner() const;
};

#endif //REACTOR_CHANNEL_H
