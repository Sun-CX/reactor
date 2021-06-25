//
// Created by suncx on 2020/8/18.
//

#include "Acceptor.h"
#include "GnuExt.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "ConsoleStream.h"
#include <fcntl.h>
#include <cassert>
#include <unistd.h>

using std::bind;

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port) :
        loop(loop),
        server_socket(),
        accept_channel(loop, server_socket.fd()),
        listening(false),
        idle_fd(open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    INFO << "create idle_fd: " << idle_fd;
    server_socket.reuse_addr(true);
    server_socket.reuse_port(reuse_port);
    server_socket.bind(addr);
    accept_channel.set_read_callback(bind(&Acceptor::read_handler, this));
}

Acceptor::~Acceptor() {
    accept_channel.disable_all();
    accept_channel.remove();
    auto status = ::close(idle_fd);
    if (unlikely(status < 0)) ERROR << "close idle fd " << idle_fd << " error!";
}

void Acceptor::read_handler() {
    assert(loop->is_in_loop_thread());
    InetAddress peer_addr;
    int con_fd = server_socket.accept(peer_addr);
    if (con_fd >= 0) {// success
        INFO << "accept new connection, con_fd: " << con_fd;
        callback(con_fd, peer_addr);
    } else {// error
        if (errno == EMFILE) {
            ::close(idle_fd);
            idle_fd = server_socket.accept(peer_addr);
            ::close(idle_fd);
            idle_fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
        } else
            FATAL << "accept new connection error!";
    }
}

void Acceptor::listen() {
    assert(loop->is_in_loop_thread());
    server_socket.listen();
    listening = true;
    accept_channel.enable_reading();
}

void Acceptor::set_new_connection_callback(const NewConnectionCallback &handler) {
    callback = handler;
}

bool Acceptor::is_listening() const {
    return listening;
}