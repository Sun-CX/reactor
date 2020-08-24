//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERQUEUE_H
#define REACTOR_TIMERQUEUE_H

#include "NonCopyable.h"
#include "Timestamp.h"
#include "Channel.h"
#include "TimerId.h"
#include <vector>
#include <set>

using std::vector;
using std::set;
using std::pair;

class EventLoop;

class Channel;

class Timer;

/**
 * 定时器内部实现，不向用户代码暴露
 */
class TimerQueue final : public NonCopyable {
private:
    using TimerCallback = function<void()>;
    using Entry = pair<Timestamp, Timer *>;
    using Timers = set<Entry>;
    using ActiveTimer = pair<Timer *, int64_t>;
    using ActiveTimerSet = set<ActiveTimer>;

    EventLoop *loop;
    const int timer_fd;
    Channel timer_channel;
    Timers timers;
    ActiveTimerSet active_timers;
    bool calling_expired_timers;
    ActiveTimerSet canceled_timers;

    int timer_create() const;

    void read_timer_fd(int fd, Timestamp now);

    vector<Entry> get_expired(Timestamp now);

    void read_handler();

    void add_timer_in_loop(Timer *timer);

    void cancel_in_loop(TimerId timer_id);

    bool insert(Timer *timer);

    void reset_timer_fd(int fd, Timestamp timestamp);

    timespec time_from_now(Timestamp timestamp);

public:
    explicit TimerQueue(EventLoop *loop);

    TimerId add_timer(TimerCallback callback, Timestamp when, double interval);

    void cancel(TimerId timer_id);
};

#endif //REACTOR_TIMERQUEUE_H
