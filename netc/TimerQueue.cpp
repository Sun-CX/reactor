//
// Created by suncx on 2020/8/18.
//

#include "TimerQueue.h"

int TimerQueue::timer_create() const {
    auto timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (unlikely(timer_fd == -1)) ERROR_EXIT("timerfd_create error.");
    return timer_fd;
}

TimerQueue::TimerQueue(EventLoop *loop) : loop(loop), timer_fd(timer_create()), timer_fd_channel(loop, timer_fd),
                                          timers(), calling_expired_timers(false) {

    timer_fd_channel.set_read_callback();
}

void TimerQueue::readable_handler() {
    loop->assert_in_created_thread();
    Timestamp now = Timestamp::now();
    read_timer_fd(timer_fd, now);
    vector<Entry> expired = get_expired(now);
}

void TimerQueue::read_timer_fd(int timer_fd, Timestamp now) {
    uint64_t many;
    auto n = read(timer_fd, &many, sizeof(many));
    if (unlikely(n != sizeof(many)))
        fprintf(stderr, "read timer_fd error.");
}

vector<TimerQueue::Entry> TimerQueue::get_expired(Timestamp now) {
    vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer *>(UINTPTR_MAX));


    return expired;
}


