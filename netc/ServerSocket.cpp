//
// Created by suncx on 2020/8/18.
//

#include "ServerSocket.h"
#include "Exception.h"
#include "InetAddress.h"
#include <unistd.h>
#include <cstring>
#include <netinet/tcp.h>

ServerSocket::ServerSocket() : sock_fd(create_socket()) {}

int ServerSocket::create_socket() const {
    int fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (unlikely(fd < 0)) ERROR_EXIT("socket created error.");
    printf("create sock_fd success, sock_fd: %d\n", fd);
    return fd;
}

ServerSocket::~ServerSocket() {
    auto status = close(sock_fd);
    if (unlikely(status != 0)) ERROR_EXIT("close server socket error.");
}

void ServerSocket::bind(const InetAddress &addr) const {
    auto status = ::bind(sock_fd, addr.get_sockaddr(), sizeof(sockaddr_in));
    if (unlikely(status != 0)) ERROR_EXIT("socket bind error.");
}

void ServerSocket::listen() const {
    auto status = ::listen(sock_fd, SOMAXCONN);
    if (unlikely(status != 0)) ERROR_EXIT("socket listen error.");
}

int ServerSocket::accept(InetAddress *peer_addr) const {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    int con_fd = ::accept4(sock_fd, reinterpret_cast<sockaddr *>(&addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    peer_addr->set_sockaddr(addr);
    return con_fd;
}

void ServerSocket::reuse_addr(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

void ServerSocket::tcp_no_delay(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

void ServerSocket::reuse_port(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

void ServerSocket::keep_alive(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(sock_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

int ServerSocket::fd() const {
    return sock_fd;
}