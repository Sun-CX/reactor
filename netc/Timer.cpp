//
// Created by suncx on 2020/9/11.
//

#include "Timer.h"
#include "Exception.h"
#include "CurrentThread.h"
#include "EventLoop.h"
#include <sys/timerfd.h>
#include <cstring>
#include <cassert>

using std::bind;

Timer::Timer(EventLoop *loop) : loop(loop), tasks(), timer_channel(loop, create_timer_fd()) {
    timer_channel.set_read_callback(bind(&Timer::read_handler, this));
    timer_channel.enable_reading();
}

Timer::~Timer() {
    timer_channel.disable_all();
    timer_channel.remove();
    auto status = close(timer_channel.get_fd());
    if (unlikely(status < 0)) fprintf(stderr, "timer_fd close error!\n");
}

int Timer::create_timer_fd() const {
    // 这里使用了 CLOCK_MONOTONIC, 因此在 timerfd_settime() 时只能用相对时间
    auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (unlikely(fd < 0)) ERROR_EXIT("timerfd_create error.");
    printf("%s[%d]: create timerfd: %d\n", CurrentThread::name, CurrentThread::pid, fd);
    return fd;
}

void Timer::read_handler() {
    assert(loop->is_in_loop_thread());
    assert(not tasks.empty());
    auto now = Timestamp::now();

    read_timeout_event();

    TimerTask *task;
    Timestamp expired_time = tasks.peek()->expire_time;
    do {
        task = tasks.pop();
        task->alarm();
        if (task->repeated()) {
            task->restart(task->expire_time + task->interval);
            tasks.insert(task);
        } else delete task;
    } while (not tasks.empty() and tasks.peek()->expire_time == expired_time);
    if (not tasks.empty()) reset_timer_fd(now);
}

void Timer::read_timeout_event() const {
    uint64_t exp;
    auto n = read(timer_channel.get_fd(), &exp, sizeof(exp));
    if (unlikely(n != sizeof(exp))) ERROR_EXIT("read timer_fd error.");
}

void Timer::reset_timer_fd(const Timestamp &timestamp) const {
    itimerspec new_val;
    memset(&new_val, 0, sizeof(new_val));
    auto latest = tasks.peek();
    new_val.it_value = (latest->get_expired_time() - timestamp).to_timespec();
    auto ret = timerfd_settime(timer_channel.get_fd(), 0, &new_val, nullptr);
    if (unlikely(ret < 0)) ERROR_EXIT("timerfd_settime error.");
}

void Timer::schedule(const TimerTask::TimerCallback &callback, const Timestamp &after, const Timestamp &interval) {
    auto now = Timestamp::now();
    auto new_task = new TimerTask(callback, Timestamp::now() + after, interval);
    loop->run_in_loop(bind(&Timer::add_timer_task_in_loop, this, new_task, now));
}

void Timer::add_timer_task_in_loop(TimerTask *task, const Timestamp &timestamp) {
    assert(loop->is_in_loop_thread());
    tasks.insert(task);
    reset_timer_fd(timestamp);
}
