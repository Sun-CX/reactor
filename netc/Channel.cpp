//
// Created by suncx on 2020/8/17.
//

#include "Channel.h"
#include <poll.h>

using std::shared_ptr;

Channel::Channel(EventLoop *loop, int fd) : loop(loop), fd(fd), events(0),
                                            revents(0), index(-1), tied(false), event_handling(false),
                                            add_to_loop(false) {}

void Channel::handle_event(Timestamp timestamp) {
    shared_ptr<void> guard;
    if (tied) {
        guard = tie.lock();
        if (guard)
            handle_event_with_guard(timestamp);
    } else handle_event_with_guard(timestamp);
}

void Channel::handle_event_with_guard(Timestamp timestamp) {
    event_handling = true;

    if ((revents & POLLHUP) and !(revents & POLLIN)) {
        fprintf(stderr, "fd: %d, Channel::handle_event error.", fd);
        if (close_callback) close_callback();
    }

    if (revents & (POLLERR | POLLNVAL)) {
        fprintf(stderr, "fd: %d, Channel::handle_event error.", fd);
        if (error_callback) error_callback();
    }

    if (revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (read_callback) read_callback(timestamp);
    }

    if (revents & POLLOUT) {
        if (write_callback) write_callback();
    }
    event_handling = false;
}

void Channel::update() {
    add_to_loop = true;
    loop->update_channel(this);
}

void Channel::set_read_callback(Channel::ReadEventCallback callback) {
    read_callback = move(callback);
}

void Channel::set_write_callback(Channel::EventCallback callback) {
    write_callback = move(callback);
}

void Channel::set_close_callback(Channel::EventCallback callback) {
    close_callback = move(callback);
}

void Channel::set_error_callback(Channel::EventCallback callback) {
    error_callback = move(callback);
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

bool Channel::none_events() const {
    return events == 0;
}

void Channel::remove() {
    add_to_loop = false;
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

//Channel::~Channel() {
//    if(loop->is_in_loop_thread()){
//
//    }
//}
