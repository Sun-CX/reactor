//
// Created by suncx on 2020/8/18.
//

#include "ServerSocket.h"
#include "GnuExt.h"
#include "InetAddress.h"
#include "ConsoleStream.h"
#include <unistd.h>
#include <netinet/tcp.h>
#include <cstring>

using reactor::net::ServerSocket;

ServerSocket::ServerSocket() : listen_fd(create_socket()) {}

ServerSocket::~ServerSocket() {
    if (unlikely(::close(listen_fd) < 0))
        RC_ERROR << "socket(" << listen_fd << ") close error: " << strerror(errno);
}

int ServerSocket::create_socket() const {
    int fd;
    if (unlikely((fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)) < 0))
        RC_FATAL << "socket error: " << strerror(errno);
    return fd;
}

void ServerSocket::bind(const InetAddress &addr) const {
    if (unlikely(::bind(listen_fd, addr.get_sockaddr(), sizeof(sockaddr_in)) < 0))
        RC_FATAL << "socket(" << listen_fd << ") bind error: " << strerror(errno);
}

void ServerSocket::listen() const {
    if (unlikely(::listen(listen_fd, SOMAXCONN) < 0))
        RC_FATAL << "socket(" << listen_fd << ") listen error: " << strerror(errno);

    RC_DEBUG << "socket(" << listen_fd << ") is listening...";
}

int ServerSocket::accept(InetAddress &peer_addr) const {
    socklen_t len = sizeof(peer_addr);
    int con_fd = ::accept4(listen_fd, peer_addr.get_sockaddr(), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    return con_fd;
}

void ServerSocket::reuse_addr(bool on) const {
    const int opt = on ? 1 : 0;
    if (unlikely(::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0))
        RC_FATAL << "setsockopt on socket(" << listen_fd << ") error: " << strerror(errno);
}

void ServerSocket::no_delay(bool on) const {
    const int opt = on ? 1 : 0;
    if (unlikely(setsockopt(listen_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0))
        RC_FATAL << "setsockopt on socket(" << listen_fd << ") error: " << strerror(errno);
}

void ServerSocket::reuse_port(bool on) const {
    const int opt = on ? 1 : 0;
    if (unlikely(::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0))
        RC_FATAL << "setsockopt on socket(" << listen_fd << ") error: " << strerror(errno);
}

int ServerSocket::fd() const {
    return listen_fd;
}