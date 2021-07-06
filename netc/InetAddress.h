//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_INETADDRESS_H
#define REACTOR_INETADDRESS_H

#include <netinet/in.h>
#include <string>

/* IPv4 addr static assert. */
static_assert(sizeof(sockaddr) == sizeof(sockaddr_in));
static_assert(sizeof(sockaddr) == 16u);

/* IPv6 addr static assert. */
static_assert(sizeof(sockaddr_in6) == 28u);

static_assert(offsetof(sockaddr_in, sin_family) == 0);
static_assert(offsetof(sockaddr_in6, sin6_family) == 0);

namespace reactor::net {
    using std::string;

    class InetAddress final {
    private:
        friend class Acceptor;

        friend class ServerSocket;

        friend class Connector;

        using u16 = uint16_t;

        union {
            sockaddr_in ad4;
            sockaddr_in6 ad6;
        };

        InetAddress();

        [[nodiscard]]
        const sockaddr *get_sockaddr() const;

        [[nodiscard]]
        sockaddr *get_sockaddr();

    public:
        InetAddress(const char *ip, u16 port, bool ipv6 = false);

        explicit InetAddress(u16 port, bool loopback_only = false, bool ipv6 = false);

        /* get protocol family. */
        /* AF_INET or AF_INET6 is returned. */
        [[nodiscard]]
        sa_family_t family() const;

        /* get a string in format <ip>:<port>. */
        [[nodiscard]]
        string to_string() const;

        static InetAddress get_local_address(int fd);
    };
}

#endif //REACTOR_INETADDRESS_H
