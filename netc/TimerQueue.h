//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERQUEUE_H
#define REACTOR_TIMERQUEUE_H

#include "NonCopyable.h"
#include "Timestamp.h"
#include "Timer.h"
#include "Channel.h"
#include <vector>
#include <set>
#include <sys/timerfd.h>

using std::vector;
using std::set;
using std::pair;

class EventLoop;

class Channel;

class TimerQueue final : public NonCopyable {
private:
    using Entry = pair<Timestamp, Timer*>;
    using Timers = set<Entry>;
    using ActiveTimer = pair<Timer *, int64_t>;
    using ActiveTimerSet = set<ActiveTimer>;

    EventLoop *loop;
    const int timer_fd;
    Channel timer_fd_channel;
    Timers timers;
    ActiveTimerSet active_timers;
    bool calling_expired_timers;
    ActiveTimerSet canceled_timers;

    int timer_create() const;

    void read_timer_fd(int timer_fd, Timestamp now);

    vector<Entry> get_expired(Timestamp now);

    void readable_handler();

public:
    explicit TimerQueue(EventLoop *loop);


};


#endif //REACTOR_TIMERQUEUE_H
