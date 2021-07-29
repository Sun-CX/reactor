//
// Created by suncx on 2020/8/18.
//

#include "Acceptor.h"
#include "Ext.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "ConsoleStream.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

using reactor::net::Acceptor;
using std::placeholders::_1;

Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port) :
        loop(loop),
        server_socket(),
        accept_channel(loop, server_socket.fd()),
        listening(false),
        idle_fd(open_idle_fd()) {
    server_socket.reuse_addr(true);
    server_socket.reuse_port(reuse_port);
    server_socket.bind(addr);
    accept_channel.on_read(bind(&Acceptor::handle_read, this, _1));
}

Acceptor::~Acceptor() {
    accept_channel.remove();
    close_idle_fd();
}

int Acceptor::open_idle_fd() const {
    int idle;
    if (unlikely((idle = ::open("/dev/null", O_RDONLY | O_CLOEXEC)) < 0))
        RC_FATAL << "open idle file error: " << ::strerror(errno);

    return idle;
}

void Acceptor::close_idle_fd() const {
    if (unlikely(::close(idle_fd) < 0))
        RC_FATAL << "close idle fd(" << idle_fd << ") error: " << ::strerror(errno);
}

void Acceptor::handle_read(const Timestamp ts) {
    assert(loop->is_in_created_thread());
    InetAddress peer_addr;
    int con_fd = server_socket.accept(peer_addr);

    if (con_fd >= 0) {

        handler(con_fd, peer_addr);

    } else {

        if (errno == EMFILE) {

            RC_WARN << "accept error: " << ::strerror(errno);

            close_idle_fd();

            idle_fd = server_socket.accept(peer_addr);

            close_idle_fd();

            idle_fd = open_idle_fd();

        } else
            RC_FATAL << "accept error: " << ::strerror(errno);
    }
}

void Acceptor::listen() {
    assert(loop->is_in_created_thread());
    server_socket.listen();
    listening = true;
    accept_channel.enable_reading();
}

void Acceptor::on_connect(const NewConnectionHandler &hdr) {
    handler = hdr;
}

bool Acceptor::is_listening() const {
    return listening;
}


