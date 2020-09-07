//
// Created by suncx on 2020/9/7.
//

#include "Socket.h"
#include "Exception.h"
#include <unistd.h>
#include <sys/socket.h>

Socket::Socket(int con_fd) : fd(con_fd) {}

Socket::~Socket() {
    auto status = close(fd);
    if (unlikely(status != 0)) ERROR_EXIT("close socket error.");
}

int Socket::get_fd() const {
    return fd;
}

void Socket::keep_alive(bool on) const {
    const int opt = on ? 1 : 0;
    auto status = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
    if (unlikely(status != 0)) ERROR_EXIT("setsockopt error.");
}

void Socket::shutdown_read() const {
    auto status = shutdown(fd, SHUT_RD);
    if (unlikely(status != 0)) ERROR_EXIT("shutdown_read error.");
}

void Socket::shutdown_write() const {
    auto status = shutdown(fd, SHUT_WR);
    if (unlikely(status != 0)) ERROR_EXIT("shutdown_write error.");
}
