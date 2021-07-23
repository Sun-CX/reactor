//
// Created by suncx on 2020/9/11.
//

#include "Timer.h"
#include "Exception.h"
#include "Thread.h"
#include "EventLoop.h"
#include "ConsoleStream.h"
#include "Ext.h"
#include <sys/timerfd.h>
#include <cstring>
#include <cassert>
#include <unistd.h>

using std::bind;
using std::chrono::duration_cast;
using std::chrono_literals::operator ""ns;
using reactor::net::Timer;
using reactor::net::Task;

Timer::Timer(EventLoop *loop) :
        loop(loop),
        tasks(),
        timer_channel(this->loop, create_timer_fd()) {

    timer_channel.set_read_callback(bind(&Timer::read_handler, this));
    timer_channel.enable_reading();
}

Timer::~Timer() {
    timer_channel.remove();
    if (unlikely(::close(timer_channel.get_fd()) < 0))
        RC_FATAL << "close timer(" << timer_channel.get_fd() << ") error: " << ::strerror(errno);
}

int Timer::create_timer_fd() const {
    int fd;

    // A nonsettable monotonically increasing clock that measures time from some unspecified point in the past that does
    // not change after system startup.
    if (unlikely((fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) < 0))
        RC_FATAL << "create timer error: " << ::strerror(errno);

    return fd;
}

void Timer::read_handler() {
    assert(loop->is_in_created_thread());
    assert(not tasks.empty());
    read_timeout_event();

    auto expire = tasks.peek()->expire;
    TimerTask *task;
    do {
        task = tasks.pop();
        task->alarm();
        if (task->is_repeated()) {
            task->restart(task->expire + task->interval);
            tasks.push(task);
            reset_timer_fd();
        } else delete task;
    } while (!tasks.empty() and tasks.peek()->expire == expire);
}

void Timer::read_timeout_event() const {
    uint64_t exp;
    ssize_t n = ::read(timer_channel.get_fd(), &exp, sizeof(exp));
    if (unlikely(n != sizeof(exp)))
        RC_FATAL << "timer(" << timer_channel.get_fd() << ") read error";
}

void Timer::reset_timer_fd() const {
    itimerspec its;

    // disable interval
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    auto latest = tasks.peek();

    its.it_value.tv_sec = duration_cast<seconds>(latest->expire.time_since_epoch()).count();
    its.it_value.tv_nsec = latest->expire.time_since_epoch().count() % 1000000000;

    int ret = ::timerfd_settime(timer_channel.get_fd(), TFD_TIMER_ABSTIME, &its, nullptr);
    if (unlikely(ret < 0))
        RC_FATAL << "timer(" << timer_channel.get_fd() << ") settime error: " << ::strerror(errno);
}

Task Timer::schedule(const TimerTask::TimerCallback &callback, const nanoseconds &after, const nanoseconds &interval) {

    TimerTask *task = nullptr;

    if (after == 0ns) { // run instantly.

        // no need to create a timer task.
        if (interval == 0ns) {
            loop->run_in_loop(callback);
        } else {
            // create a new task and run it at first time.
            task = new TimerTask(callback, steady_clock::now(), interval);
            loop->run_in_loop(bind(&Timer::run_task_in_loop, this, task));
        }
    } else {
        // create a new task and add it to `tasks`.
        task = new TimerTask(callback, steady_clock::now() + after, interval);
        loop->run_in_loop(bind(&Timer::add_task_in_loop, this, task));
    }

    return reinterpret_cast<Task>(task);
}

void Timer::run_task_in_loop(TimerTask *task) {
    assert(loop->is_in_created_thread());
    assert(task->is_repeated());

    task->alarm();

    task->restart(task->expire + task->interval);

    if (insert(task))
        reset_timer_fd();
}

void Timer::add_task_in_loop(TimerTask *task) {
    assert(loop->is_in_created_thread());

    if (insert(task))
        reset_timer_fd();
}

bool Timer::insert(TimerTask *task) {
    assert(task != nullptr);
    if (tasks.empty()) {
        tasks.push(task);
        return true;
    } else {
        const TimerTask *latest = tasks.peek();
        tasks.push(task);
        return *task < *latest;
    }
}

void Timer::cancel(Task task) {

    auto ptr = reinterpret_cast<TimerTask *>(task);
    if (ptr == nullptr) return;

    int index = ptr->get_index();
    assert(index != -1);

    // if (index == 0) {
    //
    //
    // } else {
    //     tasks.remove(index);
    // }

}
