//
// Created by suncx on 2020/9/11.
//

#ifndef REACTOR_TIMERTASK_H
#define REACTOR_TIMERTASK_H

#include "Timestamp.h"
#include <functional>

namespace reactor::net {
    using std::function;
    using reactor::core::Timestamp;

    class TimerTask final {
    private:
        friend class Timer;

        friend class EventLoop;

        using TimerCallback = function<void()>;
        Timestamp expire_time;  // 定时任务到期的绝对时间
        const Timestamp interval;
        const TimerCallback callback; // 定时任务回调

        void restart(const Timestamp &abs_time);

    public:
        TimerTask(TimerCallback callback, const Timestamp &expire_time, const Timestamp &interval);

        void alarm();

        [[nodiscard]]
        bool repeated() const;

        bool operator<(const TimerTask &rhs) const;
    };
}

#endif //REACTOR_TIMERTASK_H