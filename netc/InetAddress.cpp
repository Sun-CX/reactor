//
// Created by suncx on 2020/8/18.
//

#include "InetAddress.h"
#include "Exception.h"
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>

InetAddress::InetAddress() {
    memset(&ad4, 0, sizeof(ad4));
}

const sockaddr *InetAddress::get_sockaddr() const {
    return reinterpret_cast<const sockaddr *>(&ad4);
}

void InetAddress::set_sockaddr(const sockaddr_in &addr) {
    ad4 = addr;
}

InetAddress::InetAddress(const char *ip, uint16_t port) {
    memset(&ad4, 0, sizeof(ad4));
    ad4.sin_family = AF_INET;
    ad4.sin_addr.s_addr = inet_addr(ip);
    ad4.sin_port = htons(port);
}

InetAddress::InetAddress(bool use_loop_back, uint16_t port) {
    memset(&ad4, 0, sizeof(ad4));
    ad4.sin_family = AF_INET;
    in_addr_t ip = use_loop_back ? INADDR_LOOPBACK : INADDR_ANY;
    ad4.sin_addr.s_addr = htonl(ip);
    ad4.sin_port = htons(port);
}

string to_readable_string(const InetAddress &addr) {
    char buf[64];
    const char *ip = inet_ntoa(addr.ad4.sin_addr);
    const auto port = ntohs(addr.ad4.sin_port);
    snprintf(buf, sizeof(buf), "%s:%hu", ip, port);
    return buf;
}

InetAddress InetAddress::get_local_address(int fd) {
    InetAddress value;
    socklen_t len = sizeof(value.ad4);
    auto status = getsockname(fd, reinterpret_cast<sockaddr *>(&value.ad4), &len);
    if (unlikely(status == -1)) ERROR_EXIT("error occurred.");
    return value;
}