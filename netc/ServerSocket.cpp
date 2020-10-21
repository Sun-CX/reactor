//
// Created by suncx on 2020/8/18.
//

#include "ServerSocket.h"
#include "Exception.h"
#include "InetAddress.h"
#include "ConsoleStream.h"
#include <unistd.h>
#include <cstring>
#include <netinet/tcp.h>

ServerSocket::ServerSocket() : listen_fd(create_listen_fd()) {}

ServerSocket::~ServerSocket() {
    auto status = ::close(listen_fd);
    if (unlikely(status != 0)) ERROR << "close server socket error.";
}

int ServerSocket::create_listen_fd() const {
    int fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (unlikely(fd < 0)) FATAL << "server socket created error.";
    INFO << "create listen_fd: " << fd;
    return fd;
}

void ServerSocket::bind(const InetAddress &addr) const {
    auto status = ::bind(listen_fd, addr.get_sockaddr(), sizeof(sockaddr_in));
    if (unlikely(status != 0)) FATAL << "socket " << listen_fd << " bind error!";
}

void ServerSocket::listen() const {
    auto status = ::listen(listen_fd, SOMAXCONN);
    if (unlikely(status != 0)) FATAL << "socket " << listen_fd << " listen error!";
}

int ServerSocket::accept(InetAddress &peer_addr) const {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    int con_fd = ::accept4(listen_fd, reinterpret_cast<sockaddr *>(&addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    peer_addr.set_sockaddr(addr);
    return con_fd;
}

void ServerSocket::reuse_addr(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (unlikely(status != 0)) FATAL << "server socket " << listen_fd << " reuse_addr error!";
}

void ServerSocket::tcp_no_delay(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(listen_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    if (unlikely(status != 0)) FATAL << "server socket " << listen_fd << " tcp_no_delay error!";
}

void ServerSocket::reuse_port(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if (unlikely(status != 0)) FATAL << "server socket " << listen_fd << " reuse_port error!";
}

int ServerSocket::fd() const {
    return listen_fd;
}