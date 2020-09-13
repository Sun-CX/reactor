//
// Created by suncx on 2020/8/17.
//

#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>
#include <sys/epoll.h>

static_assert(POLLIN == EPOLLIN and
              POLLPRI == EPOLLPRI and
              POLLOUT == EPOLLOUT and
              POLLWRBAND == EPOLLWRBAND and
              POLLRDNORM == EPOLLRDNORM and
              POLLRDBAND == EPOLLRDBAND and
              POLLWRNORM == EPOLLWRNORM and
              POLLERR == EPOLLERR and
              POLLHUP == EPOLLHUP and
              POLLMSG == EPOLLMSG and
              POLLRDHUP == EPOLLRDHUP
//              POLLREMOVE == EPOLLREMOVE
//              POLLNVAL == EPOLLNVAL
        , "POLL/EPOLL constants assert failed.");

Channel::Channel(EventLoop *loop, int fd) : loop(loop), fd(fd), events(0), revents(0), index(-1) {}

void Channel::update() {
    loop->update_channel(this);
}

void Channel::handle_events() {
//    event_handling = true;
    if (revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (read_callback) read_callback();
    }

    if (revents & POLLOUT) {
        if (write_callback) write_callback();
    }

    if ((revents & POLLHUP) and !(revents & POLLIN)) {
        if (close_callback) close_callback();
    }

    if (revents & (POLLERR | POLLNVAL)) {
        if (error_callback) error_callback();
    }
//    event_handling = false;
}

uint32_t Channel::get_events() const {
    return events;
}

void Channel::set_revents(uint32_t ev) {
    revents = ev;
}

bool Channel::none_events_watched() const {
    return events == 0;
}

void Channel::remove() {
    loop->remove_channel(this);
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

int Channel::get_fd() const {
    return fd;
}

bool Channel::reading_enabled() const {
    return events & (POLLIN | POLLPRI);
}

bool Channel::writing_enabled() const {
    return events & (POLLOUT | POLLWRBAND);
}