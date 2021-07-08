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

thread_local EventLoop *EventLoop::current_thread_loop = nullptr;
const int EventLoop::default_poll_timeout_milliseconds = -1;   // 默认永不超时

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

    RC_INFO << "---------------------- EventLoop constructed ----------------------";

    if (unlikely(current_thread_loop != nullptr)) {
        RC_FATAL << "current thread already has an event loop object(" << current_thread_loop << ')';
    } else current_thread_loop = this;

    wakeup_channel->set_read_callback(bind(&EventLoop::read_wakeup_event, this));
    wakeup_channel->enable_reading();
}

EventLoop::~EventLoop() {
    RC_INFO << "---------------------- ~EventLoop ----------------------";
    wakeup_channel->disable_all();
    wakeup_channel->remove();
    if (unlikely(::close(wakeup_channel->get_fd()) < 0))
        RC_FATAL << "close eventfd(" << wakeup_channel->get_fd() << ") error: " << strerror(errno);
    current_thread_loop = nullptr;
}

void EventLoop::loop() {
    assert(is_in_loop_thread());
    looping = true;
    RC_INFO << "EventLoop(" << this << ") start loop...";
    while (!exited) {
        active_channels.clear();
        poller->poll(active_channels, default_poll_timeout_milliseconds);
        for_each(active_channels.cbegin(), active_channels.cend(), bind(&Channel::handle_events, _1));
        // IO 事件处理完毕之后再执行的动作
        execute_pending_functors();
    }
    looping = false;
    RC_INFO << "EventLoop(" << this << ") stop loop...";
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
    return pid == CurrentThread::id;
}

void EventLoop::quit() {
    exited = true;
    if (!is_in_loop_thread()) wakeup();
}

void EventLoop::run_in_loop(const Functor &func) {
    RC_INFO << __func__ << " invoked, loop in: " << CurrentThread::name;
    is_in_loop_thread() ? func() : queue_in_loop(func);
}

void EventLoop::queue_in_loop(const Functor &func) {
    RC_INFO << __func__ << " invoked, loop in: " << CurrentThread::name;
    {
        MutexGuard guard(mutex);
        pending_functors.push_back(func);
    }
    if (!is_in_loop_thread() or calling_pending_func) wakeup();
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
    return current_thread_loop;
}

int EventLoop::create_event_fd() const {
    int fd;
    if (unlikely((fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)) < 0))
        RC_FATAL << "create eventfd error: " << strerror(errno);
    return fd;
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    auto n = write(wakeup_channel->get_fd(), &one, sizeof(one));
    if (unlikely(n != sizeof(one))) RC_FATAL << "write to eventfd " << wakeup_channel->get_fd() << " error!";
    RC_INFO << "write event to eventfd " << wakeup_channel->get_fd();
}

void EventLoop::read_wakeup_event() const {
    uint64_t one;
    auto n = read(wakeup_channel->get_fd(), &one, sizeof(one));
    assert(n == sizeof(one));
    RC_INFO << "receive event from eventfd " << wakeup_channel->get_fd();
}

void EventLoop::schedule(const TimerTask::TimerCallback &callback, const Timestamp &after, const Timestamp &interval) {
    timer->schedule(callback, after, interval);
}