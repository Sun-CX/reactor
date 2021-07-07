//
// Created by suncx on 2020/8/18.
//

#include "InetAddress.h"
#include "GnuExt.h"
#include "ConsoleStream.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <cassert>

using std::string;
using reactor::net::InetAddress;

InetAddress::InetAddress() {
    ::memset(&ad6, 0, sizeof(ad6));
}

InetAddress::InetAddress(const char *ip, u16 port, bool ipv6) {
    if (unlikely(ipv6)) {
        ::memset(&ad6, 0, sizeof(ad6));
        ad6.sin6_family = AF_INET6;

        int rc = ::inet_pton(AF_INET6, ip, &ad6.sin6_addr);
        if (unlikely(rc == 0)) {
            RC_FATAL << "Invalid ipv6 address: " << ip;
        } else if (unlikely(rc < 0)) {
            RC_FATAL << "System error, errno = " << errno;
        }

        assert(rc == 1);
        ad6.sin6_port = htons(port);
    } else {
        ::memset(&ad4, 0, sizeof(ad4));
        ad4.sin_family = AF_INET;
        ad4.sin_addr.s_addr = ::inet_addr(ip);
        ad4.sin_port = ::htons(port);
    }
}

InetAddress::InetAddress(u16 port, bool loopback_only, bool ipv6) {
    if (unlikely(ipv6)) {
        ::memset(&ad6, 0, sizeof(ad6));
        ad6.sin6_family = AF_INET6;
        in6_addr ip = loopback_only ? in6addr_loopback : in6addr_any;
        ad6.sin6_addr = ip;
        ad6.sin6_port = ::htons(port);
    } else {
        ::memset(&ad4, 0, sizeof(ad4));
        ad4.sin_family = AF_INET;
        in_addr_t ip = loopback_only ? INADDR_LOOPBACK : INADDR_ANY;
        ad4.sin_addr.s_addr = ::htonl(ip);
        ad4.sin_port = ::htons(port);
    }
}

sa_family_t InetAddress::family() const {
    return ad4.sin_family;
}

const sockaddr *InetAddress::get_sockaddr() const {
    return reinterpret_cast<const sockaddr *>(&ad6);
}

sockaddr *InetAddress::get_sockaddr() {
    return reinterpret_cast<sockaddr *>(&ad6);
}

InetAddress InetAddress::get_local_address(int fd) {
    InetAddress value;
    socklen_t len = sizeof(value);
    static_assert(sizeof(sockaddr_in6) == sizeof(value));

    auto status = ::getsockname(fd, reinterpret_cast<sockaddr *>(&value.ad6), &len);
    if (unlikely(status < 0)) RC_FATAL << "getsockname error!";
    return value;
}

InetAddress InetAddress::get_peer_address(int fd) {
    InetAddress value;
    socklen_t len = sizeof(value);
    static_assert(sizeof(sockaddr_in6) == sizeof(value));

    auto status = ::getpeername(fd, reinterpret_cast<sockaddr *>(&value.ad6), &len);
    if (unlikely(status < 0)) RC_FATAL << "getpeername error!";
    return value;
}

string InetAddress::to_string() const {
    char buf[64];

    if (unlikely(family() == AF_INET6)) {
        const char *r = ::inet_ntop(AF_INET6, &ad6.sin6_addr, buf, sizeof(buf));
        assert(r != nullptr);
        size_t len = ::strlen(buf);
        const u16 port = ::ntohs(ad6.sin6_port);
        ::snprintf(buf + len, sizeof(buf) - len, ":%hu", port);
    } else {
        const char *ip = ::inet_ntoa(ad4.sin_addr);
        const u16 port = ::ntohs(ad4.sin_port);
        ::snprintf(buf, sizeof(buf), "%s:%hu", ip, port);
    }
    return buf;
}

InetAddress InetAddress::resolve(const char *hostname) {

    static thread_local char resolve_buf[64 * 1024];

    InetAddress val;

    hostent het;
    hostent *hs = nullptr;
    int err;

    int rc = ::gethostbyname_r(hostname, &het, resolve_buf, sizeof(resolve_buf), &hs, &err);
    if (rc == 0 and hs != nullptr) {
//        assert(hs->h_addrtype == AF_INET);
        assert(hs->h_length == sizeof(int));

        if (unlikely(hs->h_addrtype == AF_INET6)) {
            RC_WARN << "IPv6 not implemented.";
        } else {
            val.ad4.sin_family = AF_INET;
            val.ad4.sin_addr = *reinterpret_cast<in_addr *>(hs->h_addr);
        }
    } else {
        RC_FATAL << "resolve " << hostname << " failed";
    }
    return val;
}

string InetAddress::ip_string() const {
    if (unlikely(family() == AF_INET6)) {
        char buf[64];
        const char *r = ::inet_ntop(AF_INET6, &ad6.sin6_addr, buf, sizeof(buf));
        assert(r != nullptr);
        return buf;
    } else {
        return ::inet_ntoa(ad4.sin_addr);
    }
}


