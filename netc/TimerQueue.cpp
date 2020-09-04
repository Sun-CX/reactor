//
// Created by suncx on 2020/8/18.
//

#include "TimerQueue.h"
#include "EventLoop.h"
#include "Exception.h"
#include "Timer.h"
#include "TimerId.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

using std::bind;
using std::copy;

static_assert(UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF, "UINTPTR_MAX assert failed.");

TimerQueue::TimerQueue(EventLoop *loop) : loop(loop), timer_fd(create_timer_fd()), timer_channel(loop, timer_fd),
                                          calling_expired_timers(false) {
    timer_channel.set_read_callback(bind(&TimerQueue::read_handler, this));
    timer_channel.enable_reading();
}

int TimerQueue::create_timer_fd() const {
    auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (unlikely(fd < 0)) ERROR_EXIT("timerfd_create error.");
    return fd;
}

void TimerQueue::reset_timer_fd(int fd, Timestamp timestamp) const {
    itimerspec new_val;
    itimerspec old_val;
    memset(&new_val, 0, sizeof(new_val));
    memset(&old_val, 0, sizeof(old_val));
    new_val.it_value = time_from_now(timestamp);
    auto ret = timerfd_settime(fd, 0, &new_val, &old_val);
    if (unlikely(ret < 0)) ERROR_EXIT("timerfd_settime error.");
}

void TimerQueue::read_timeout_event(int fd, Timestamp now) const {
    uint64_t exp;
    auto n = read(fd, &exp, sizeof(exp));
#ifndef NDEBUG
    printf("TimerQueue handle readable event at %s\n", now.to_fmt_string().c_str());
#endif
    if (unlikely(n != sizeof(exp))) fprintf(stderr, "read timer_fd error.");
}

timespec TimerQueue::time_from_now(Timestamp time) const {
    auto ms = time.microseconds_since_epoch() - Timestamp::now().microseconds_since_epoch();
    if (ms < 100) ms = 100;
    return {ms / Timestamp::microseconds_per_second, ms % Timestamp::microseconds_per_second * 1000};
}

void TimerQueue::read_handler() {
    assert(loop->is_in_loop_thread());
    Timestamp now = Timestamp::now();
    read_timeout_event(timer_fd, now);
    vector<Entry> expired = get_expired(now);
    calling_expired_timers = true;
    canceled_timers.clear();

    for (const Entry &e:expired) {
        e.second->run();
    }
    calling_expired_timers = false;
    reset(expired, now);
}

vector<TimerQueue::Entry> TimerQueue::get_expired(Timestamp now) {
    vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));
    auto end = timers.lower_bound(sentry);
    assert(end == timers.end() or now < end->first);

    copy(timers.cbegin(), end, back_inserter(expired));

    timers.erase(timers.cbegin(), end);

    for (const Entry &e:expired) {
        ActiveTimer timer(e.second, e.second->get_sequence());
        auto n = active_timers.erase(timer);
        assert(n == 1);
    }

    assert(timers.size() == active_timers.size());
    return expired;
}

TimerId TimerQueue::add_timer(TimerQueue::TimerCallback callback, Timestamp when, double interval) {
    auto timer = new Timer(move(callback), when, interval);
    loop->run_in_loop(bind(&TimerQueue::add_timer_in_loop, this, timer));
    return {timer, timer->get_sequence()};
}

void TimerQueue::add_timer_in_loop(Timer *timer) {
    assert(loop->is_in_loop_thread());
    // 插入一个定时器，可能会使最早到期的定时器发生改变
    bool is_earliest_timer_task = insert(timer);
    if (is_earliest_timer_task) reset_timer_fd(timer_fd, timer->expire_time());
}

bool TimerQueue::insert(Timer *timer) {
    assert(loop->is_in_loop_thread());
    assert(timers.size() == active_timers.size());
    bool earliest_changed = false;
    auto when = timer->expire_time();
    // 如果 timers 为空或者新插入的定时器的超时时刻小于已有的定时器的最小时刻
    if (timers.empty() or when < timers.begin()->first) {
        earliest_changed = true;
    }

    auto result = timers.insert(Entry(when, timer));
    assert(result.second);

    auto re = active_timers.insert({timer, timer->get_sequence()});
    assert(re.second);

    assert(timers.size() == active_timers.size());
    return earliest_changed;
}

void TimerQueue::cancel(TimerId timer_id) {
    loop->run_in_loop(bind(&TimerQueue::cancel_in_loop, this, timer_id));
}

void TimerQueue::cancel_in_loop(TimerId timer_id) {
    assert(loop->is_in_loop_thread());
    ActiveTimer active_timer(timer_id.timer, timer_id.sequence);
    auto it = active_timers.find(active_timer);
    if (it != active_timers.end()) {
        auto n = timers.erase(Entry(it->first->expire_time(), it->first));
        assert(n == 1);
        delete it->first;
        active_timers.erase(it);
    } else if (calling_expired_timers) {
        canceled_timers.insert(active_timer);
    }
    assert(timers.size() == active_timers.size());
}

void TimerQueue::reset(vector<Entry> &expired, Timestamp now) {
    Timestamp next_expire;
    for (auto it = expired.cbegin(); it != expired.cend();) {
        ActiveTimer timer(it->second, it->second->get_sequence());
        if (it->second->repeated() and canceled_timers.find(timer) == canceled_timers.end()) {
            it->second->restart(now);
            insert(it->second);
            it++;
        } else {
            delete it->second;
            it = expired.erase(it);
        }
    }
    if (!timers.empty()) next_expire = timers.begin()->second->expire_time();
    if (next_expire.valid()) reset_timer_fd(timer_fd, next_expire);
}