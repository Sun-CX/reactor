//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include "NonCopyable.h"
#include <functional>

namespace reactor::net {
    using std::function;
    using reactor::core::NonCopyable;

    class EventLoop;

    class Channel final : public NonCopyable {
    private:
        using EventCallback  = function<void()>;

        EventLoop *const loop;

        // file descriptor listened by this channel.
        const int fd;

        // watched events used by poll/epoll system call.
        uint32_t events;

        // triggered events actually.
        uint32_t revents;

        // field `index` has two meanings in different mechanisms:
        // * in poll mechanism, `index` is index of channel in vector<pollfd>.
        // * in epoll mechanism, `index` is one of two different states(defined in `EpollPoller.h`) of channel.
        int index;

        bool events_handling;

        EventCallback read_callback;
        EventCallback write_callback;
        EventCallback close_callback;
        EventCallback error_callback;

        void update();

    public:
        Channel(EventLoop *loop, int fd);

        ~Channel();

        void handle_events();

        [[nodiscard]]
        int get_fd() const;

        void set_index(int idx);

        [[nodiscard]]
        int get_index() const;

        [[nodiscard]]
        uint32_t get_events() const;

        void set_revents(uint32_t e);

        [[nodiscard]]
        bool is_disabled() const;

        [[nodiscard]]
        EventLoop *loop_owner() const;

        void enable_reading();

        void disable_reading();

        void enable_writing();

        void disable_writing();

        // watching no events, but once channel was put into poll or epoll mechanism, POLLHUP and POLLERR may also be triggered.
        void disable();

        // remove channel from poll or epoll mechanism, then any events won't be triggered.
        void remove();

        [[nodiscard]]
        bool reading_enabled() const;

        [[nodiscard]]
        bool writing_enabled() const;

        void set_read_callback(const EventCallback &callback);

        void set_write_callback(const EventCallback &callback);

        void set_close_callback(const EventCallback &callback);

        void set_error_callback(const EventCallback &callback);
    };
}

#endif //REACTOR_CHANNEL_H
