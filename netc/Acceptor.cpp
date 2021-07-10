//
// Created by suncx on 2020/8/18.
//

#include "Acceptor.h"
#include "GnuExt.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "ConsoleStream.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

using std::bind;
using reactor::net::Acceptor;

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port) :
        loop(loop),
        server_socket(),
        accept_channel(loop, server_socket.fd()),
        listening(false),
        idle_fd(open_idle_fd()) {
    server_socket.reuse_addr(true);
    server_socket.reuse_port(reuse_port);
    server_socket.bind(addr);
    accept_channel.set_read_callback(bind(&Acceptor::read_handler, this));
}

Acceptor::~Acceptor() {
    accept_channel.disable();
    accept_channel.remove();
    close_idle_fd();
}

int Acceptor::open_idle_fd() const {
    int idle;
    if (unlikely((idle = ::open("/dev/null", O_RDONLY | O_CLOEXEC)) < 0))
        RC_FATAL << "open idle file error: " << strerror(errno);

    return idle;
}

void Acceptor::close_idle_fd() const {
    if (unlikely(::close(idle_fd) < 0))
        RC_FATAL << "close idle fd(" << idle_fd << ") error: " << strerror(errno);
}

void Acceptor::read_handler() {
    assert(loop->is_in_created_thread());
    InetAddress peer_addr;
    int con_fd = server_socket.accept(peer_addr);
    if (con_fd >= 0) {// success
        callback(con_fd, peer_addr);
    } else {// error
        if (errno == EMFILE) {

            close_idle_fd();

            idle_fd = server_socket.accept(peer_addr);

            close_idle_fd();

            idle_fd = open_idle_fd();

        } else
            RC_FATAL << "accept new connection error!";
    }
}

void Acceptor::listen() {
    assert(loop->is_in_created_thread());
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


