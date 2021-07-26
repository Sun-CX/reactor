//
// Created by suncx on 2020/9/11.
//

#ifndef REACTOR_TIMERTASK_H
#define REACTOR_TIMERTASK_H

#include "NonCopyable.h"
#include <functional>
#include <chrono>

namespace reactor::net {
    using reactor::core::NonCopyable;
    using std::function;
    using std::chrono::steady_clock;
    using std::chrono::nanoseconds;

    class TimerTask final : public NonCopyable {
    private:
        using TimerCallback = function<void()>;

        friend class EventLoop;

        friend class Timer;

        friend class TimerHeap;

        // expired time point of timer task.
        steady_clock::time_point expire;

        nanoseconds interval;
        const TimerCallback timer_callback;

        // index of current task in Timer's `tasks`.
        // index = -1 means not in `tasks`.
        int index;

        void restart(steady_clock::time_point point);

    public:

        /**
         * create a timer task.
         * @param callback timer callback when timeout.
         * @param expire time_point of timeout.
         * @param interval set a periodic task.
         */
        TimerTask(TimerCallback callback, steady_clock::time_point expire, nanoseconds interval);

        ~TimerTask();

        void alarm();

        [[nodiscard]]
        bool is_repeated() const;

        [[nodiscard]]
        steady_clock::time_point get_expire() const;

        [[nodiscard]]
        int get_index() const;

        bool operator<(const TimerTask &rhs) const;
    };
}

#endif //REACTOR_TIMERTASK_H