//
// Created by suncx on 2020/8/17.
//

#include "EventLoop.h"
#include "Exception.h"
#include "Poller.h"
#include "Timer.h"
#include "ConsoleStream.h"
#include <sys/eventfd.h>
#include <cassert>

using std::bind;
using std::placeholders::_1;

thread_local EventLoop *EventLoop::loop_in_this_thread;
const int EventLoop::default_poll_timeout_milliseconds = -1;   // 默认永不超时

EventLoop::EventLoop() : looping(false), exited(false), thread_name(CurrentThread::name), pid(CurrentThread::pid),
                         poller(Poller::default_poller(this)), mutex(), calling_pending_func(false),
                         wakeup_channel(new Channel(this, create_event_fd())), timer(new Timer(this)) {
    if (unlikely(loop_in_this_thread != nullptr)) {
        FATAL << "this thread already has an event loop object(" << loop_in_this_thread << ')';
    } else loop_in_this_thread = this;

    wakeup_channel->set_read_callback(bind(&EventLoop::read_wakeup_event, this));
    wakeup_channel->enable_reading();
}

EventLoop::~EventLoop() {
    wakeup_channel->disable_all();
    wakeup_channel->remove();
    auto status = ::close(wakeup_channel->get_fd());
    if (unlikely(status < 0))
        ERROR << "eventfd " << wakeup_channel->get_fd() << " close error!";
    loop_in_this_thread = nullptr;
}

void EventLoop::loop() {
    assert(is_in_loop_thread());
    LOG << "EventLoop(" << this << ") start loop...";
    looping = true;
    while (!exited) {
        active_channels.clear();
        poller->poll(&active_channels, default_poll_timeout_milliseconds);
        for_each(active_channels.cbegin(), active_channels.cend(), bind(&Channel::handle_events, _1));
        // IO 事件处理完毕之后再执行的动作
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
    LOG << __func__ << " invoked, loop in: " << thread_name << "[" << pid << "]";
    is_in_loop_thread() ? func() : queue_in_loop(func);
}

void EventLoop::queue_in_loop(const Functor &func) {
    LOG << __func__ << " invoked, loop in: " << thread_name << "[" << pid << "]";
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
    return loop_in_this_thread;
}

int EventLoop::create_event_fd() const {
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (unlikely(fd < 0)) FATAL << "create eventfd failed.";
    INFO << "create eventfd: " << fd;
    return fd;
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    auto n = write(wakeup_channel->get_fd(), &one, sizeof(one));
    if (unlikely(n != sizeof(one))) FATAL << "write to eventfd " << wakeup_channel->get_fd() << " error!";
    INFO << "write event to eventfd " << wakeup_channel->get_fd();
}

void EventLoop::read_wakeup_event() const {
    uint64_t one;
    auto n = read(wakeup_channel->get_fd(), &one, sizeof(one));
    if (unlikely(n != sizeof(one)))
        FATAL << "read from eventfd " << wakeup_channel->get_fd() << " error!";//ERROR_EXIT("read error.");
    INFO << "receive event from eventfd " << wakeup_channel->get_fd();
}

void EventLoop::schedule(const TimerTask::TimerCallback &callback, const Timestamp &after, const Timestamp &interval) {
    timer->schedule(callback, after, interval);
}