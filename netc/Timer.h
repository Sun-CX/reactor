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
    class EventLoop;

    class Timer final : public NonCopyable {
    private:
        EventLoop *const loop;
        TimerHeap tasks;
        Channel timer_channel;

        [[nodiscard]]
        int create_timer_fd() const;

        void handle_read(Timestamp ts);

        void read_timeout_event() const;

        void set_timer() const;

        void clear_timer() const;

        void run_task_in_loop(const shared_ptr<TimerTask> &task);

        void add_task_in_loop(const shared_ptr<TimerTask> &task);

        void cancel_task_in_loop(const shared_ptr<TimerTask> &task);

        // insert a timer task into `tasks`.
        // return true if the insert one has latest expire time.
        bool insert(const shared_ptr<TimerTask> &task);

    public:
        explicit Timer(EventLoop *loop);

        ~Timer();

        // cross-thread calling is allowed.
        shared_ptr<TimerTask> schedule(const TimerTask::TimerCallback &callback, nanoseconds delay, nanoseconds interval);

        // cross-thread calling is allowed.
        void cancel(const shared_ptr<TimerTask> &task);
    };
}

#endif //REACTOR_TIMER_H
