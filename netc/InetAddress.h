//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_INETADDRESS_H
#define REACTOR_INETADDRESS_H

#include <cstring>
#include <netinet/in.h>
#include "../logging/StringPiece.h"

class InetAddress final {
private:

    union {
        sockaddr_in ad4;
        sockaddr_in6 ad6;
    };

public:
    explicit InetAddress(uint16_t port = 0, bool loop_back_only = false, bool ipv6 = false);

    InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);

    const sockaddr *get_sockaddr() const;

    void set_sockaddr(const sockaddr_in6 &ad6);
};

#endif //REACTOR_INETADDRESS_H
