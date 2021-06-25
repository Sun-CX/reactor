//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include "NonCopyable.h"
#include <functional>

using std::function;

class EventLoop;

// Channel 是对文件描述符 IO 事件的注册和响应的封装
class Channel final : public NonCopyable {
private:
    using EventCallback  = function<void()>;

    EventLoop *loop;
    const int fd;       // Channel 只引用 fd，并不管理 fd 的创建和关闭
    uint32_t events;    // 关注的 IO 事件
    uint32_t revents;   // 发生的 IO 事件
    /*
     * 在 PollPoller 的实现中是 channel 在 PollPoller::fds 中的索引
     * 在 EpollPoller 的实现中是代表 channel 的三种不同状态
     */
    int index;

    EventCallback read_callback;
    EventCallback write_callback;
    EventCallback close_callback;
    EventCallback error_callback;

    void update();

public:
    Channel(EventLoop *loop, int fd);

    void handle_events();

    void set_read_callback(const EventCallback &callback);

    void set_write_callback(const EventCallback &callback);

    void set_close_callback(const EventCallback &callback);

    void set_error_callback(const EventCallback &callback);

    [[nodiscard]]
    int get_fd() const;

    void set_index(int idx);

    [[nodiscard]]
    int get_index() const;

    [[nodiscard]]
    uint32_t get_events() const;

    void set_revents(uint32_t e);

    [[nodiscard]]
    bool no_events_watched() const;

    [[nodiscard]]
    EventLoop *loop_owner() const;

    void enable_reading();

    void disable_reading();

    void enable_writing();

    void disable_writing();

    void disable_all();

    void remove();

    [[nodiscard]]
    bool reading_enabled() const;

    [[nodiscard]]
    bool writing_enabled() const;
};

#endif //REACTOR_CHANNEL_H
