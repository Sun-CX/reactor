//
// Created by suncx on 2020/8/17.
//

#include "Channel.h"
#include "EventLoop.h"
#include <cstdio>
#include <poll.h>

//using std::shared_ptr;

Channel::Channel(EventLoop *loop, int fd) : loop(loop), fd(fd), events(0), revents(0), index(-1) {}

//void Channel::handle_event() {
//    shared_ptr<void> guard;
//    if (tied) {
//        guard = tie.lock();
//        if (guard)
//            handle_event_with_guard(timestamp);
//    } else handle_event_with_guard(timestamp);
//}

void Channel::handle_event() {
//    event_handling = true;

    if ((revents & POLLHUP) and !(revents & POLLIN)) {
        fprintf(stderr, "fd: %d, Channel::handle_event error.", fd);
        if (close_callback) close_callback();
    }

    if (revents & (POLLERR | POLLNVAL)) {
        fprintf(stderr, "fd: %d, Channel::handle_event error.", fd);
        if (error_callback) error_callback();
    }

    if (revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (read_callback) read_callback();
    }

    if (revents & POLLOUT) {
        if (write_callback) write_callback();
    }
//    event_handling = false;
}

void Channel::update() {
//    add_to_loop = true;
    loop->update_channel(this);
}

int Channel::get_fd() const {
    return fd;
}

int Channel::get_events() const {
    return events;
}

void Channel::set_revents(int ev) {
    revents = ev;
}

bool Channel::has_none_events() const {
    return events == 0;
}

void Channel::remove() {
//    add_to_loop = false;
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
    events |= (POLLIN | POLLPRI);
    update();
}

void Channel::disable_reading() {
    events &= ~(POLLIN | POLLPRI);
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

//Channel::~Channel() {
//    if(loop->is_in_loop_thread()){
//
//    }
//}
