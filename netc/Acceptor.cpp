//
// Created by suncx on 2020/8/18.
//

#include "Acceptor.h"
#include "Exception.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

using std::bind;

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port) :
        loop(loop), socket(create_socket()), accept_channel(loop, socket.fd()),
        listening(false), idle_fd(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    socket.reuse_addr(true);
    socket.reuse_port(reuse_port);
    socket.bind(addr);
    accept_channel.set_read_callback(bind(&Acceptor::read_handler, this));
}

int Acceptor::create_socket() const {
    int sock_fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (unlikely(sock_fd < 0)) ERROR_EXIT("socket created error.");
    printf("create sock_fd success, sock_fd: %d\n", sock_fd);
    return sock_fd;
}

void Acceptor::read_handler() {
    loop->assert_in_created_thread();
    InetAddress peer_addr;
    int con_fd = socket.accept(&peer_addr);
    if (con_fd >= 0) {// success
        if (callback) callback(con_fd, peer_addr);
        else {
            fprintf(stderr, "Acceptor ConnectionCallback not set, please check.");
            close(con_fd);
        }
    } else {// error
        if (errno == EMFILE) {
            close(idle_fd);
            idle_fd = socket.accept(&peer_addr);
            close(idle_fd);
            idle_fd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
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

void Acceptor::set_new_conn_callback(const Acceptor::ConnectionCallback &handler) {
    callback = handler;
}

void Acceptor::listen() {
    loop->assert_in_created_thread();
    listening = true;
    socket.listen();
    accept_channel.enable_reading();
}

bool Acceptor::is_listening() const {
    return listening;
}
