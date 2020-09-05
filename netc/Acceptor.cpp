//
// Created by suncx on 2020/8/18.
//

#include "Acceptor.h"
#include "Exception.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include <fcntl.h>
#include <cassert>

using std::bind;

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port) :
        loop(loop), socket(), accept_channel(loop, socket.fd()),
        listening(false), idle_fd(open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    socket.reuse_addr(true);
    socket.reuse_port(reuse_port);
    socket.bind(addr);
    accept_channel.set_read_callback(bind(&Acceptor::read_handler, this));
}

void Acceptor::read_handler() {
    assert(loop->is_in_loop_thread());
    InetAddress peer_addr;
    int con_fd = socket.accept(&peer_addr);
    if (con_fd >= 0) {// success
        if (callback) callback(con_fd, peer_addr);
        else {
            fprintf(stderr, "Acceptor ConnectionCallback not set, close this connection.\n");
            close(con_fd);
        }
    } else {// error
        if (errno == EMFILE) {
            close(idle_fd);
            idle_fd = socket.accept(&peer_addr);
            close(idle_fd);
            idle_fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
        } else {
            fprintf(stderr, "con_fd error.\n");
        }
    }
}

Acceptor::~Acceptor() {
    accept_channel.disable_all();
    accept_channel.remove();
    close(idle_fd);
}

void Acceptor::set_connection_callback(const Acceptor::ConnectionCallback &handler) {
    callback = handler;
}

void Acceptor::listen() {
    assert(loop->is_in_loop_thread());
    listening = true;
    socket.listen();
    accept_channel.enable_reading();
}

bool Acceptor::is_listening() const {
    return listening;
}