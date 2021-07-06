//
// Created by suncx on 2020/8/18.
//

#include "ServerSocket.h"
#include "GnuExt.h"
#include "InetAddress.h"
#include "ConsoleStream.h"
#include <unistd.h>
#include <netinet/tcp.h>

using reactor::net::ServerSocket;

ServerSocket::ServerSocket() : listen_fd(create_listen_fd()) {}

ServerSocket::~ServerSocket() {
    auto status = ::close(listen_fd);
    if (unlikely(status != 0)) RC_ERROR << "close server socket error.";
}

int ServerSocket::create_listen_fd() const {
    int fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (unlikely(fd < 0)) RC_FATAL << "server socket created error.";
    RC_INFO << "create listen_fd: " << fd;
    return fd;
}

void ServerSocket::bind(const InetAddress &addr) const {
    auto status = ::bind(listen_fd, addr.get_sockaddr(), sizeof(sockaddr_in));
    if (unlikely(status != 0)) RC_FATAL << "socket " << listen_fd << " bind error!";
}

void ServerSocket::listen() const {
    auto status = ::listen(listen_fd, SOMAXCONN);
    if (unlikely(status != 0)) RC_FATAL << "socket " << listen_fd << " listen error!";
}

int ServerSocket::accept(InetAddress &peer_addr) const {
    socklen_t len = sizeof(peer_addr);
    int con_fd = ::accept4(listen_fd, peer_addr.get_sockaddr(), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    return con_fd;
}

void ServerSocket::reuse_addr(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (unlikely(status != 0)) RC_FATAL << "server socket " << listen_fd << " reuse_addr error!";
}

void ServerSocket::tcp_no_delay(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(listen_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    if (unlikely(status != 0)) RC_FATAL << "server socket " << listen_fd << " tcp_no_delay error!";
}

void ServerSocket::reuse_port(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if (unlikely(status != 0)) RC_FATAL << "server socket " << listen_fd << " reuse_port error!";
}

int ServerSocket::fd() const {
    return listen_fd;
}