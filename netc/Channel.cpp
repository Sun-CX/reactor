//
// Created by suncx on 2020/8/17.
//

#include "Channel.h"
#include "EventLoop.h"
#include "ConsoleStream.h"
#include <poll.h>
#include <sys/epoll.h>
#include <cassert>

Channel::Channel(EventLoop *loop, int fd) : loop(loop), fd(fd), events(0), revents(0), index(-1) {}

void Channel::update() {
    loop->update_channel(this);
}

void Channel::remove() {
    loop->remove_channel(this);
}

void Channel::handle_events() {
    if (revents & (POLLIN | POLLPRI | POLLRDHUP)) {
//        INFO << "************** POLLIN | POLLPRI | POLLRDHUP(" << fd << ") **************";
        assert(read_callback);
        read_callback();
    }

    if (revents & POLLOUT) {
//        INFO << "************** POLLOUT(" << fd << ") **************";
        assert(write_callback);
        write_callback();
    }

    if (revents & POLLHUP and !(revents & POLLIN)) {
//        INFO << "************** POLLHUP and !POLLIN(" << fd << ") **************";
        assert(close_callback);
        close_callback();
    }

    if (revents & (POLLERR | POLLNVAL)) {
//        INFO << "************** POLLERR | POLLNVAL(" << fd << ") **************";
        assert(error_callback);
        error_callback();
    }
}

uint32_t Channel::get_events() const {
    return events;
}

void Channel::set_revents(uint32_t e) {
    revents = e;
}

bool Channel::no_events_watched() const {
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
    events |= POLLIN | POLLPRI;
    update();
}

void Channel::disable_reading() {
    events &= ~(POLLIN | POLLPRI);
    update();
}

void Channel::enable_writing() {
    events |= POLLOUT | POLLWRBAND;
    update();
}

void Channel::disable_writing() {
    events &= ~(POLLOUT | POLLWRBAND);
    update();
}

void Channel::disable_all() {
    events = 0;
    update();
}

int Channel::get_fd() const {
    return fd;
}

bool Channel::reading_enabled() const {
    return events & (POLLIN | POLLPRI);
}

bool Channel::writing_enabled() const {
    return events & (POLLOUT | POLLWRBAND);
}

void Channel::set_read_callback(const Channel::EventCallback &callback) {
    read_callback = callback;
}

void Channel::set_write_callback(const Channel::EventCallback &callback) {
    write_callback = callback;
}

void Channel::set_close_callback(const Channel::EventCallback &callback) {
    close_callback = callback;
}

void Channel::set_error_callback(const Channel::EventCallback &callback) {
    error_callback = callback;
}