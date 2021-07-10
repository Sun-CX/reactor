//
// Created by suncx on 2020/8/17.
//

#include "EventLoop.h"
#include "Thread.h"
#include "GnuExt.h"
#include "Poller.h"
#include "Timer.h"
#include "ConsoleStream.h"
#include <sys/eventfd.h>
#include <cassert>
#include <unistd.h>
#include <algorithm>
#include <cstring>

using std::bind;
using std::placeholders::_1;
using std::for_each;
using reactor::net::EventLoop;
using reactor::core::CurrentThread;
using reactor::core::MutexGuard;

thread_local EventLoop *EventLoop::eventloop_in_current_thread = nullptr;
const int EventLoop::default_timeout_milliseconds = -1;   // 默认永不超时

EventLoop::EventLoop() :
        looping(false),
        exited(false),
        thread_name(CurrentThread::name),
        pid(CurrentThread::id),
        poller(Poller::default_poller(this)),
        active_channels(),
        mutex(),
        calling_pending_func(false),
        pending_functors(),
        wakeup_channel(new Channel(this, create_event_fd())),
        timer(new Timer(this)) {

    if (likely(eventloop_in_current_thread == nullptr)) {
        eventloop_in_current_thread = this;

        wakeup_channel->set_read_callback(bind(&EventLoop::read_wakeup, this));
        wakeup_channel->enable_reading();
        RC_DEBUG << "---------------------- +EventLoop ----------------------";
    } else
        RC_FATAL << CurrentThread::name << " already exists an eventloop object, one thread has one eventloop object at most.";
}

EventLoop::~EventLoop() {
    wakeup_channel->disable();
    wakeup_channel->remove();
    close_event_fd(wakeup_channel->get_fd());

    eventloop_in_current_thread = nullptr;
    RC_DEBUG << "---------------------- -EventLoop ----------------------";
}

void EventLoop::loop() {
    assert(is_in_created_thread());
    looping = true;
    RC_DEBUG << "start loop...";
    while (!exited) {
        active_channels.clear();
        poller->poll(active_channels, default_timeout_milliseconds);
        // RC_DEBUG << "active_channels' size: " << active_channels.size();
        for_each(active_channels.cbegin(), active_channels.cend(), bind(&Channel::handle_events, _1));

        execute_pending_functors();
    }
    looping = false;
    RC_DEBUG << "stop loop...";
}

void EventLoop::update_channel(Channel *channel) {
    assert(is_in_created_thread());
    assert(channel->loop_owner() == this);
    poller->update_channel(channel);
}

void EventLoop::remove_channel(Channel *channel) {
    assert(is_in_created_thread());
    assert(channel->loop_owner() == this);
    poller->remove_channel(channel);
}

bool EventLoop::has_channel(Channel *channel) {
    assert(is_in_created_thread());
    assert(channel->loop_owner() == this);
    return poller->has_channel(channel);
}

bool EventLoop::is_in_created_thread() const {
    return pid == CurrentThread::id;
}

void EventLoop::quit() {
    exited = true;
    if (!is_in_created_thread()) wakeup();
}

void EventLoop::run_in_loop(const Functor &func) {
    is_in_created_thread() ? func() : queue_in_loop(func);
}

void EventLoop::queue_in_loop(const Functor &func) {
    {
        MutexGuard guard(mutex);
        pending_functors.push_back(func);
    }
    if (!is_in_created_thread() or calling_pending_func) wakeup();
}

void EventLoop::execute_pending_functors() {
    Functors fns;
    calling_pending_func = true;
    {
        MutexGuard guard(mutex);
        fns.swap(pending_functors);
    }
    // RC_DEBUG << "pending_functors' size: " << fns.size();

    for (const auto &functor : fns)
        functor();

    calling_pending_func = false;
}

EventLoop *EventLoop::eventloop_of_current_thread() {
    return eventloop_in_current_thread;
}

int EventLoop::create_event_fd() const {
    int fd;
    if (unlikely((fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)) < 0))
        RC_FATAL << "create eventfd error: " << strerror(errno);
    return fd;
}

void EventLoop::close_event_fd(int fd) const {
    if (unlikely(::close(fd) < 0))
        RC_FATAL << "close eventfd(" << wakeup_channel->get_fd() << ") error: " << strerror(errno);
}

void EventLoop::wakeup() const {
    uint64_t value = 1u;
    auto n = ::write(wakeup_channel->get_fd(), &value, sizeof(value));
    if (unlikely(n != sizeof(value)))
        RC_FATAL << "write to eventfd(" << wakeup_channel->get_fd() << ") error";
    RC_DEBUG << "write to eventfd(" << wakeup_channel->get_fd() << "): value = " << value;
}

void EventLoop::read_wakeup() const {
    uint64_t value;
    auto n = ::read(wakeup_channel->get_fd(), &value, sizeof(value));
    assert(n == sizeof(value));
    RC_DEBUG << "read from eventfd(" << wakeup_channel->get_fd() << "): value = " << value;
}

void EventLoop::schedule(const TimerTask::TimerCallback &callback, const Timestamp &after, const Timestamp &interval) {
    timer->schedule(callback, after, interval);
}


