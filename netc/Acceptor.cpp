//
// Created by suncx on 2020/8/18.
//

#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port) :
loop(loop), accept_socket(create_socket()), accept_channel(loop, accept_socket.fd()),
listening(false), idle_fd(::open("/dev/null",O_RDONLY | O_CLOEXEC)) {
    accept_socket.reuse_addr(true);
    accept_socket.reuse_port(reuse_port);
    accept_socket.bind(addr);
    accept_channel.set_read_callback(bind(&Acceptor::read_handler, this));
}

int Acceptor::create_socket() const {
    int sock_fd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (unlikely(sock_fd == -1)) ERROR_EXIT("socket created error.");
    return sock_fd;
}

void Acceptor::read_handler() {
    loop->assert_in_created_thread();
    InetAddress peer_addr;
    int con_fd = accept_socket.accept(&peer_addr);
    if (callback) callback(con_fd, peer_addr);
    else close(con_fd);
}

Acceptor::~Acceptor() {
    accept_channel.disable_all();
    accept_channel.remove();
    close(idle_fd);
}

void Acceptor::set_read_handler(const Acceptor::ConnectionCallback &handler) {
    callback = handler;
}

void Acceptor::listen() {
    loop->assert_in_created_thread();
    listening = true;
    accept_socket.listen();
    accept_channel.enable_reading();
}

bool Acceptor::is_listening() const {
    return listening;
}
