//
// Created by suncx on 2020/8/18.
//

#include "TimerQueue.h"
#include "EventLoop.h"
#include "Exception.h"
#include "Timer.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <cassert>
#include <cstring>

using std::bind;

int TimerQueue::timer_create() const {
    auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (unlikely(fd == -1)) ERROR_EXIT("timerfd_create error.");
    return fd;
}

TimerQueue::TimerQueue(EventLoop *loop) : loop(loop), timer_fd(timer_create()), timer_channel(loop, timer_fd),
                                          timers(), calling_expired_timers(false) {

    timer_channel.set_read_callback(bind(&TimerQueue::read_handler, this));
    timer_channel.enable_reading();
}

void TimerQueue::read_handler() {
    loop->assert_in_created_thread();
    Timestamp now = Timestamp::now();
    read_timer_fd(timer_fd, now);
    vector<Entry> expired = get_expired(now);
}

void TimerQueue::read_timer_fd(int fd, Timestamp now) {
    uint64_t many;
    auto n = read(fd, &many, sizeof(many));
    if (unlikely(n != sizeof(many)))
        fprintf(stderr, "read timer_fd error.");
}

vector<TimerQueue::Entry> TimerQueue::get_expired(Timestamp now) {
    vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));


    return expired;
}

TimerId TimerQueue::add_timer(TimerQueue::TimerCallback callback, Timestamp when, double interval) {
    auto new_timer = new Timer(move(callback), when, interval);
    loop->run_in_loop(bind(&TimerQueue::add_timer_in_loop, this, new_timer));
    return {new_timer, new_timer->get_sequence()};
}

void TimerQueue::add_timer_in_loop(Timer *timer) {
    loop->assert_in_created_thread();
    auto earliest_changed = insert(timer);
    if (earliest_changed)
        reset_timer_fd(timer_fd, timer->expire_time());
}

bool TimerQueue::insert(Timer *timer) {
    loop->assert_in_created_thread();
    assert(timers.size() == active_timers.size());
    bool earliest_changed = false;
    auto when = timer->expire_time();
    if (timers.begin() == timers.end() or when < timers.begin()->first) {
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
    loop->assert_in_created_thread();
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

void TimerQueue::reset_timer_fd(int fd, Timestamp timestamp) {
    itimerspec new_val;
    itimerspec old_val;
    memset(&new_val, 0, sizeof(new_val));
    memset(&old_val, 0, sizeof(old_val));
    new_val.it_value = time_from_now(timestamp);
    auto ret = timerfd_settime(timer_fd, 0, &new_val, &old_val);
    if (unlikely(ret != 0)) ERROR_EXIT("timerfd_settime error.");
}


