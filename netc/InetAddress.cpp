//
// Created by suncx on 2020/8/18.
//

#include "InetAddress.h"

InetAddress::InetAddress(uint16_t port, bool loop_back_only, bool ipv6) {
    if (ipv6) {
        memset(&ad6, 0, sizeof(ad6));
        ad6.sin6_family = AF_INET6;
        in6_addr ip = loop_back_only ? in6addr_loopback : in6addr_any;
        ad6.sin6_addr = ip;
        ad6.sin6_port = htobe16(port);
    } else {
        memset(&ad4, 0, sizeof(ad4));;
        ad4.sin_family = AF_INET;
        in_addr_t ip = loop_back_only ? INADDR_LOOPBACK : INADDR_ANY;
        ad4.sin_addr.s_addr = htobe32(ip);
        ad4.sin_port = htobe16(port);
    }
}

InetAddress::InetAddress(StringArg ip, uint16_t port, bool ipv6) {
    if (ipv6) {

    } else {
        memset(&ad4, 0, sizeof(ad4));

    }
}

const sockaddr *InetAddress::get_sockaddr() const {
    return reinterpret_cast<const sockaddr *>(&ad6);
}

void InetAddress::set_sockaddr(const sockaddr_in6 &ad6) {
    this->ad6 = ad6;
}
