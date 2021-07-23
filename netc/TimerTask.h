//
// Created by suncx on 2020/9/11.
//

#ifndef REACTOR_TIMERTASK_H
#define REACTOR_TIMERTASK_H

#include "NonCopyable.h"
#include <functional>
#include <chrono>

namespace reactor::net {
    using std::function;
    using std::chrono::steady_clock;
    using std::chrono::nanoseconds;
    using reactor::core::NonCopyable;

    class TimerTask final : public NonCopyable {
    private:
        using TimerCallback = function<void()>;

        friend class Timer;

        friend class TimerHeap;

        friend class EventLoop;

        // expired time point of timer task.
        steady_clock::time_point expire;
        const nanoseconds interval;
        const TimerCallback callback;
        // index of current task in Timer's `tasks`.
        // index = -1 means not in `tasks`.
        int index;

        void set_index(int i);

        void restart(const steady_clock::time_point &point);

    public:
        TimerTask(TimerCallback callback, const steady_clock::time_point &expire, const nanoseconds &interval);

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