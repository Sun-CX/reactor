//
// Created by suncx on 2020/8/17.
//

#include "EventLoop.h"
#include "Exception.h"
#include "Poller.h"
#include "Timer.h"
#include <sys/eventfd.h>
#include <cassert>

using std::bind;
using std::placeholders::_1;

thread_local EventLoop *EventLoop::loop_in_this_thread;
const int EventLoop::default_poll_timeout_milliseconds = -1;   // 默认永不超时

EventLoop::EventLoop() : looping(false), exited(false), pid(CurrentThread::pid), poller(Poller::default_poller(this)),
                         mutex(), calling_pending_func(false), wakeup_channel(new Channel(this, create_event_fd())),
                         timer(new Timer(this)) {
    if (unlikely(loop_in_this_thread != nullptr)) ERROR_EXIT("Current thread already has a event loop!");
    else loop_in_this_thread = this;

    wakeup_channel->set_read_callback(bind(&EventLoop::read_wakeup_event, this));
    wakeup_channel->enable_reading();
}

EventLoop::~EventLoop() {
    wakeup_channel->disable_all();
    wakeup_channel->remove();
    auto status = close(wakeup_channel->get_fd());
    if (unlikely(status < 0)) fprintf(stderr, "event_fd close error!\n");
    loop_in_this_thread = nullptr;
}

void EventLoop::loop() {
    assert(is_in_loop_thread());
    printf("%s[%d]: EventLoop(%p) start loop...\n", CurrentThread::name, CurrentThread::pid, this);
    looping = true;
    while (not exited) {
        active_channels.clear();
        poller->poll(&active_channels, default_poll_timeout_milliseconds);
        for_each(active_channels.cbegin(), active_channels.cend(), bind(&Channel::handle_events, _1));
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
#ifndef NDEBUG
    printf("%s[%d]: is_in_loop_thread: %s\n", CurrentThread::name, CurrentThread::pid,
           is_in_loop_thread() ? "true" : "false");
#endif
    is_in_loop_thread() ? func() : queue_in_loop(func);
}

void EventLoop::queue_in_loop(const Functor &func) {
    {
        MutexGuard guard(mutex);
        pending_functors.push_back(func);
    }
    if (not is_in_loop_thread() or calling_pending_func) wakeup();
}

void EventLoop::execute_pending_functors() {
    Functors fns;
    calling_pending_func = true;
    {
        MutexGuard guard(mutex);
        fns.swap(pending_functors);
    }
    for (const auto &functor:fns) functor();
    calling_pending_func = false;
}

EventLoop *EventLoop::event_loop_of_current_thread() {
    return loop_in_this_thread;
}

int EventLoop::create_event_fd() const {
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (unlikely(fd < 0)) ERROR_EXIT("eventfd created failed.");
    printf("%s[%d]: create eventfd: %d\n", CurrentThread::name, CurrentThread::pid, fd);
    return fd;
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    auto n = write(wakeup_channel->get_fd(), &one, sizeof(one));
    if (unlikely(n != sizeof(one))) ERROR_EXIT("write error.");
    printf("%s[%d]: wakeup invoked, write something into eventfd %d.\n", CurrentThread::name, CurrentThread::pid,
           wakeup_channel->get_fd());
}

void EventLoop::read_wakeup_event() const {
    uint64_t one;
    auto n = read(wakeup_channel->get_fd(), &one, sizeof(one));
    if (unlikely(n != sizeof(one))) ERROR_EXIT("read error.");
    printf("%s[%d]: receive event, read something from eventfd %d.\n", CurrentThread::name, CurrentThread::pid,
           wakeup_channel->get_fd());
}

void EventLoop::schedule(const TimerTask::TimerCallback &callback, const Timestamp &after, const Timestamp &interval) {
    timer->schedule(callback, after, interval);
}