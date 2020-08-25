//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include "NonCopyable.h"
#include <functional>
#include <memory>

using std::function;

class EventLoop;

/**
 * Channel 是对 IO 事件注册和响应的封装
 *
 * 每个 Channel 对象自始至终只属于一个 EventLoop（即只属于某一个 IO 线程），负责某一个文件描述符的 IO 事件分发
 */
class Channel final : public NonCopyable {
private:
    using EventCallback  = function<void()>;

    EventLoop *loop;
    const int fd;        // Channel 本身不持有 fd，由用户代码控制 fd 的生存期
    uint32_t events;     // 关注的 IO 事件
    uint32_t revents;    // 实际发生了的 IO 事件
    int index;      // 当前 Channel 对象在 PollPoller::fds 中的索引

//    weak_ptr<void> tie;
//    bool tied;
//    bool event_handling;
//    bool add_to_loop;

    EventCallback read_callback;
    EventCallback write_callback;
    EventCallback close_callback;
    EventCallback error_callback;

//    void handle_event_with_guard(Timestamp timestamp);

    void update();

public:
    Channel(EventLoop *loop, int fd);

    void handle_event();

    void set_read_callback(const EventCallback &callback);

    void set_write_callback(const EventCallback &callback);

    void set_close_callback(const EventCallback &callback);

    void set_error_callback(const EventCallback &callback);

    int get_fd() const;

    void set_index(int idx);

    int get_index() const;

    uint32_t get_events() const;

    void set_revents(uint32_t ev);

    bool has_none_events() const;

    void remove();

    EventLoop *loop_owner() const;

    void enable_reading();

    void disable_reading();

    void enable_writing();

    void disable_writing();

    void disable_all();

    bool is_writing() const;

    bool is_reading() const;
};

#endif //REACTOR_CHANNEL_H
