//
// Created by suncx on 2020/8/18.
//

#include "Socket.h"
#include "Exception.h"
#include "InetAddress.h"
#include <unistd.h>
#include <cstring>
#include <netinet/tcp.h>

Socket::Socket(int sock_fd) : sock_fd(sock_fd) {}

Socket::~Socket() {
    auto status = close(sock_fd);
    if (unlikely(status != 0)) ERROR_EXIT("close socket error.");
}

void Socket::bind(const InetAddress &addr) {
    auto status = ::bind(sock_fd, addr.get_sockaddr(), sizeof(sockaddr_in));
    if (unlikely(status != 0)) ERROR_EXIT("socket bind error.");
}

void Socket::listen() {
    auto status = ::listen(sock_fd, SOMAXCONN);
    if (unlikely(status != 0)) ERROR_EXIT("socket listen error.");
}

int Socket::accept(InetAddress *peer_addr) {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    int con_fd = ::accept4(sock_fd, reinterpret_cast<sockaddr *>(&addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    peer_addr->set_sockaddr(addr);
    return con_fd;
}

void Socket::reuse_addr(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

void Socket::tcp_no_delay(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

void Socket::reuse_port(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

void Socket::keep_alive(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

int Socket::fd() const {
    return sock_fd;
}


