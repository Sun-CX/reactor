//
// Created by suncx on 2020/8/17.
//

#include "Channel.h"
#include "EventLoop.h"
#include "ConsoleStream.h"
#include <poll.h>
#include <sys/epoll.h>
#include <cassert>

using reactor::net::Channel;
using reactor::net::EventLoop;

static_assert(POLLIN == EPOLLIN);
static_assert(POLLOUT == EPOLLOUT);
static_assert(POLLRDHUP == EPOLLRDHUP);
static_assert(POLLPRI == EPOLLPRI);
static_assert(POLLERR == EPOLLERR);
static_assert(POLLHUP == EPOLLHUP);

// static_assert(POLLNVAL == EPOLLNVAR);

Channel::Channel(EventLoop *loop, int fd) :
        loop(loop),
        fd(fd),
        events(0),
        revents(0),
        index(-1),
        events_handling(false) {
    RC_DEBUG << "---------------------- +Channel(" << this->fd << ") ----------------------";
}

Channel::~Channel() {
    RC_DEBUG << "---------------------- -Channel(" << fd << ") ----------------------";
    assert(!events_handling);
}

void Channel::update() {
    loop->update_channel(this);
}

void Channel::remove() {
    loop->remove_channel(this);
}

void Channel::handle_events() {

    events_handling = true;

    RC_DEBUG << "====================== begin ======================";

    if (revents & (POLLIN | POLLPRI)) {
        assert(read_callback);
        RC_DEBUG << "Read triggered.";
        read_callback();
    }

    if (revents & POLLOUT) {
        assert(write_callback);
        RC_DEBUG << "Write triggered.";
        write_callback();
    }

    if (revents & (POLLRDHUP | POLLHUP)) {
        assert(close_callback);
        RC_DEBUG << "Close triggered.";
        close_callback();
    }

    if (revents & POLLERR) {
        assert(error_callback);
        RC_DEBUG << "Error triggered.";
        error_callback();
    }

    RC_DEBUG << "====================== end ======================";

    events_handling = false;
}

uint32_t Channel::get_events() const {
    return events;
}

void Channel::set_revents(uint32_t e) {
    revents = e;
}

bool Channel::is_disabled() const {
    return events == 0;
}

EventLoop *Channel::loop_owner() const {
    return loop;
}

int Channel::get_index() const {
    return index;
}

void Channel::set_index(int idx) {
    index = idx;
}

void Channel::enable_reading() {
    events |= POLLIN | POLLPRI | POLLRDHUP;
    update();
}

void Channel::disable_reading() {
    events &= ~(POLLIN | POLLPRI | POLLRDHUP);
    update();
}

void Channel::enable_writing() {
    events |= POLLOUT;
    update();
}

void Channel::disable_writing() {
    events &= ~POLLOUT;
    update();
}

void Channel::disable() {
    events = 0;
    update();
}

int Channel::get_fd() const {
    return fd;
}

bool Channel::reading_enabled() const {
    return events & (POLLIN | POLLPRI | POLLRDHUP);
}

bool Channel::writing_enabled() const {
    return events & POLLOUT;
}

void Channel::set_read_callback(const EventCallback &callback) {
    read_callback = callback;
}

void Channel::set_write_callback(const EventCallback &callback) {
    write_callback = callback;
}

void Channel::set_close_callback(const EventCallback &callback) {
    close_callback = callback;
}

void Channel::set_error_callback(const EventCallback &callback) {
    error_callback = callback;
}