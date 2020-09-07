//
// Created by suncx on 2020/8/18.
//

#include "Timer.h"
#include "EventLoop.h"
#include "Exception.h"
#include "TimerId.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

using std::bind;
using std::copy;

static_assert(UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF, "UINTPTR_MAX assert failed.");

Timer::Timer(EventLoop *loop) : loop(loop), timer_fd(create_timer_fd()), timer_channel(loop, timer_fd),
                                calling_expired_timers(false) {
    timer_channel.set_read_callback(bind(&Timer::read_handler, this));
    timer_channel.enable_reading();
}

Timer::~Timer() {
    timer_channel.disable_all();
    timer_channel.remove();
    auto status = close(timer_fd);
    if (unlikely(status < 0)) fprintf(stderr, "timer_fd close error!\n");
    for (const Entry &e:timers) delete e.second;
}

int Timer::create_timer_fd() const {
    auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (unlikely(fd < 0)) ERROR_EXIT("timerfd_create error.");
    printf("create timerfd success, timerfd: %d\n", fd);
    return fd;
}

void Timer::reset_timer_fd(int fd, Timestamp timestamp) const {
    itimerspec new_val;
    memset(&new_val, 0, sizeof(new_val));
    new_val.it_value = time_from_now(timestamp);
    auto ret = timerfd_settime(fd, 0, &new_val, nullptr);
    if (unlikely(ret < 0)) ERROR_EXIT("timerfd_settime error.");
}

void Timer::read_timeout_event(int fd, Timestamp now) const {
    uint64_t exp;
    auto n = read(fd, &exp, sizeof(exp));
#ifndef NDEBUG
    printf("TimerQueue handle readable event at %s\n", now.to_fmt_string().c_str());
#endif
    if (unlikely(n != sizeof(exp))) fprintf(stderr, "read timer_fd error.");
}

timespec Timer::time_from_now(Timestamp time) const {
    auto ms = time.microseconds_since_epoch() - Timestamp::now().microseconds_since_epoch();
    if (ms < 100) ms = 100;
    return {ms / Timestamp::microseconds_per_second, ms % Timestamp::microseconds_per_second * 1000};
}

void Timer::read_handler() {
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

vector<Timer::Entry> Timer::get_expired(Timestamp now) {
    vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<TimerTask *>(UINTPTR_MAX));
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

TimerId Timer::schedule(TimerTask::TimerCallback callback, Timestamp when, double interval) {
    auto task = new TimerTask(move(callback), when, interval);
    loop->run_in_loop(bind(&Timer::add_timer_in_loop, this, task));
    return {task, task->get_sequence()};
}

void Timer::add_timer_in_loop(TimerTask *task) {
    assert(loop->is_in_loop_thread());
    // 插入一个定时器，可能会使最早到期的定时器发生改变
    bool is_earliest_timer_task = insert(task);
    if (is_earliest_timer_task) reset_timer_fd(timer_fd, task->expire_time());
}

bool Timer::insert(TimerTask *task) {
    assert(loop->is_in_loop_thread());
    assert(timers.size() == active_timers.size());
    bool earliest_changed = false;
    auto when = task->expire_time();
    // 如果 timers 为空或者新插入的定时器的超时时刻小于已有的定时器的最小时刻
    if (timers.empty() or when < timers.begin()->first) {
        earliest_changed = true;
    }

    auto result = timers.insert(Entry(when, task));
    assert(result.second);

    auto re = active_timers.insert({task, task->get_sequence()});
    assert(re.second);

    assert(timers.size() == active_timers.size());
    return earliest_changed;
}

void Timer::cancel(TimerId timer_id) {
    loop->run_in_loop(bind(&Timer::cancel_in_loop, this, timer_id));
}

void Timer::cancel_in_loop(TimerId timer_id) {
    assert(loop->is_in_loop_thread());
    ActiveTimer active_timer(timer_id.timer_task, timer_id.sequence);
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

void Timer::reset(vector<Entry> &expired, Timestamp now) {
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
