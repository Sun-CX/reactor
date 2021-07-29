//
// Created by suncx on 2020/9/7.
//

#include "Socket.h"
#include "Ext.h"
#include "ConsoleStream.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

using reactor::net::Socket;

Socket::Socket(int con_fd) : fd(con_fd) {}

Socket::~Socket() {
    if (unlikely(::close(fd) < 0))
        RC_FATAL << "socket(" << fd << ") close error: " << ::strerror(errno);
}

int Socket::get_fd() const {
    return fd;
}

void Socket::keep_alive(bool on) const {
    const int opt = on ? 1 : 0;
    if (unlikely(::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0))
        RC_FATAL << "setsockopt on socket(" << fd << ") error: " << ::strerror(errno);
}

void Socket::shutdown_read() const {
    if (unlikely(::shutdown(fd, SHUT_RD) < 0))
        RC_FATAL << "shutdown socket(" << fd << ") error: " << ::strerror(errno);
}

void Socket::shutdown_write() const {
    if (unlikely(::shutdown(fd, SHUT_WR) < 0))
        RC_FATAL << "shutdown socket(" << fd << ") error: " << ::strerror(errno);
}

int Socket::get_error() const {
    int opt;
    socklen_t len = sizeof(opt);
    if (unlikely(::getsockopt(fd, SOL_SOCKET, SO_ERROR, &opt, &len) < 0)) {
        RC_FATAL << "getsockopt(" << fd << ") error: " << ::strerror(errno);
        return errno;
    } else return opt;
}