//
// Created by suncx on 2020/9/11.
//

#ifndef REACTOR_TIMER_H
#define REACTOR_TIMER_H

#include "TimerTask.h"
#include "TimerHeap.h"
#include "Channel.h"
#include <chrono>

namespace reactor::net {
    using reactor::core::NonCopyable;
    using reactor::net::TimerHeap;
    using Task = uintptr_t;

    class EventLoop;

    class Timer final : public NonCopyable {
    private:
        EventLoop *const loop;
        TimerHeap tasks;
        Channel timer_channel;

        [[nodiscard]]
        int create_timer_fd() const;

        void read_handler();

        void read_timeout_event() const;

        void reset_timer_fd() const;

        void add_task_in_loop(TimerTask *task);

        bool insert(TimerTask *task);

        void run_task_in_loop(TimerTask *task);

    public:
        explicit Timer(EventLoop *loop);

        ~Timer();

        // 可跨线程调用
        Task schedule(const TimerTask::TimerCallback &callback, const nanoseconds &after, const nanoseconds &interval);

        void cancel(Task task);
    };
}

#endif //REACTOR_TIMER_H
