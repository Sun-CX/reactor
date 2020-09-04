//
// Created by suncx on 2020/8/17.
//

#include "EventLoop.h"
#include "Exception.h"
#include "Channel.h"
#include "Poller.h"
#include "TimerId.h"
#include "TimerQueue.h"
#include <sys/eventfd.h>
#include <algorithm>
#include <cassert>

using std::bind;
using std::for_each;
using std::placeholders::_1;

thread_local EventLoop *EventLoop::loop_in_this_thread;
const int EventLoop::default_timeout_milliseconds = -1;   // 默认永不超时

EventLoop::EventLoop() : looping(false), exited(false), pid(CurrentThread::pid), poller(Poller::default_poller(this)),
                         mutex(), calling_pending_func(false), event_fd(create_event_fd()),
                         wakeup_channel(new Channel(this, event_fd)), timer_queue(new TimerQueue(this)) {
    if (unlikely(loop_in_this_thread != nullptr)) {
        fprintf(stderr, "Another EventLoop already existed in %s[%d].", CurrentThread::name, pid);
        exit(0);
    } else loop_in_this_thread = this;

    wakeup_channel->set_read_callback(bind(&EventLoop::read_handler, this));
    // 关注 event_fd 可读事件
    wakeup_channel->enable_reading();
}

EventLoop::~EventLoop() {
    wakeup_channel->disable_all();
    wakeup_channel->remove();
    close(event_fd);
    loop_in_this_thread = nullptr;
}

void EventLoop::loop() {
    assert(is_in_loop_thread());

    looping = true;

    printf("EventLoop %p start loop...\n", this);

    while (!exited) {
        active_channels.clear();
        poller->poll(&active_channels, default_timeout_milliseconds);
        for_each(active_channels.cbegin(), active_channels.cend(), bind(&Channel::handle_event, _1));
        execute_pending_functors();
    }
    looping = false;
}

void EventLoop::update_channel(Channel *channel) {
    assert(is_in_loop_thread() and channel->loop_owner() == this);
    poller->update_channel(channel);
}

void EventLoop::remove_channel(Channel *channel) {
    assert(is_in_loop_thread() and channel->loop_owner() == this);
    poller->remove_channel(channel);
}

bool EventLoop::has_channel(Channel *channel) {
    assert(is_in_loop_thread() and channel->loop_owner() == this);
    return poller->has_channel(channel);
}

bool EventLoop::is_in_loop_thread() const {
    return pid == CurrentThread::pid;
}

void EventLoop::quit() {
    exited = true;
    if (!is_in_loop_thread()) wakeup();
}

void EventLoop::run_in_loop(const Functor &func) {
    is_in_loop_thread() ? func() : queue_in_loop(func);
}

void EventLoop::queue_in_loop(const EventLoop::Functor &func) {
    {
        MutexGuard guard(mutex);
        pending_functors.push_back(func);
    }
    if (!is_in_loop_thread() or calling_pending_func) wakeup();
}

EventLoop *EventLoop::event_loop_of_current_thread() {
    return loop_in_this_thread;
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    auto n = write(event_fd, &one, sizeof(one));
    if (unlikely(n != sizeof(one))) ERROR_EXIT("write error.");
}

int EventLoop::create_event_fd() const {
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (unlikely(fd < 0)) ERROR_EXIT("eventfd created failed.");
    return fd;
}

void EventLoop::read_handler() const {
    uint64_t one;
    auto n = read(event_fd, &one, sizeof(one));
    if (unlikely(n != sizeof(one))) ERROR_EXIT("read error.");
}

void EventLoop::execute_pending_functors() {
    Functors fns;
    calling_pending_func = true;
    {
        MutexGuard guard(mutex);
        fns.swap(pending_functors);
    }
    for_each(fns.cbegin(), fns.cend(), [](const Functor &func) {
        func();
    });
    calling_pending_func = false;
}

TimerId EventLoop::run_at(const Timer::TimerCallback &callback, Timestamp timestamp) {
    return timer_queue->add_timer(callback, timestamp);
}

TimerId EventLoop::run_after(const Timer::TimerCallback &callback, double delay) {
    return timer_queue->add_timer(callback, add_time(Timestamp::now(), delay));
}

TimerId EventLoop::run_every(const Timer::TimerCallback &callback, double delay) {
    return timer_queue->add_timer(callback, add_time(Timestamp::now(), delay), delay);
}

void EventLoop::cancel(TimerId id) {
    timer_queue->cancel(id);
}