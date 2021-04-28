//
// Created by suncx on 2020/9/11.
//

#include "Timer.h"
#include "Exception.h"
#include "Thread.h"
#include "EventLoop.h"
#include "ConsoleStream.h"
#include "GnuExt.h"
#include <sys/timerfd.h>
#include <cstring>
#include <cassert>
#include <unistd.h>

using std::bind;

Timer::Timer(EventLoop *loop) : loop(loop), timer_channel(this->loop, create_timer_fd()), base_time(Timestamp::now()) {
    timer_channel.set_read_callback(bind(&Timer::read_handler, this));
    timer_channel.enable_reading();
}

Timer::~Timer() {
    timer_channel.disable_all();
    timer_channel.remove();
    auto status = ::close(timer_channel.get_fd());
    if (unlikely(status < 0))
        ERROR << "timer_fd " << timer_channel.get_fd() << " close error!";
}

int Timer::create_timer_fd() const {
    auto fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
    if (unlikely(fd < 0)) FATAL << "timerfd_create error!";
    INFO << "create timer_fd: " << fd;
    return fd;
}

void Timer::read_handler() {
    assert(loop->is_in_loop_thread());
    assert(not tasks.empty());
    read_timeout_event();

    Timestamp expired_time = tasks.peek()->expire_time;
    TimerTask *task;
    do {
        task = tasks.pop();
        task->alarm();
        if (task->repeated()) {
            task->restart(task->expire_time + task->interval);
            tasks.insert(task);
            reset_timer_fd();
        } else delete task;
    } while (!tasks.empty() and tasks.peek()->expire_time == expired_time);
}

void Timer::read_timeout_event() const {
    uint64_t exp;
    auto n = read(timer_channel.get_fd(), &exp, sizeof(exp));
    if (unlikely(n != sizeof(exp))) FATAL << "read timer_fd error!";
}

void Timer::reset_timer_fd() const {
    itimerspec new_val;
    memset(&new_val, 0, sizeof(new_val));
    new_val.it_value = tasks.peek()->expire_time.to_timespec();
    auto ret = timerfd_settime(timer_channel.get_fd(), TFD_TIMER_ABSTIME, &new_val, nullptr);
    if (unlikely(ret < 0)) FATAL << "timerfd_settime error!";
}

void Timer::schedule(const TimerTask::TimerCallback &callback, const Timestamp &after, const Timestamp &interval) {
    auto new_task = new TimerTask(callback, base_time + after, interval);
    loop->run_in_loop(bind(&Timer::add_timer_task_in_loop, this, new_task));
}

void Timer::add_timer_task_in_loop(TimerTask *task) {
    assert(loop->is_in_loop_thread());
    assert(base_time <= task->expire_time);
    if (task->expire_time == base_time) {   // 立即执行
        task->callback();
        if (task->repeated()) {
            task->restart(task->expire_time + task->interval);
            if (insert(task)) reset_timer_fd();
        } else delete task;
    } else {    // 延期执行
        if (insert(task)) reset_timer_fd();
    }
}

bool Timer::insert(TimerTask *task) {
    assert(task != nullptr);
    if (tasks.empty()) {
        tasks.insert(task);
        return true;
    } else {
        TimerTask *latest = tasks.peek();
        tasks.insert(task);
        return *task < *latest;
    }
}