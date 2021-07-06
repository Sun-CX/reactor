//
// Created by suncx on 2020/8/19.
//

#include "InetAddress.h"
#include "ConsoleStream.h"

using reactor::net::InetAddress;

namespace xxx {

    /**
     * IPv4 地址结构
     */
    struct sockaddr_in {
        uint8_t sin_len;            // sockaddr_in 整个结构体的长度
        sa_family_t sin_family;     // AF_INET = IPv4, AF_INET6 = IPv6
        in_port_t sin_port;
        in_addr sin_addr;
        char sin_zero[8];   // 最后三项加起来正好为 14 byte
    };

    struct sockaddr {
        uint8_t sin_len;
        sa_family_t sin_family;
        char sa_data[14];
    };
}

int main(int argc, const char *argv[]) {

    InetAddress addr(9090, true);
    RC_DEBUG << addr.to_string();

    InetAddress ad("192.168.2.2", 8080);
    RC_DEBUG << ad.to_string();

    InetAddress ad2("ABCD:EF01:2345:6789:ABCD:EF01:2345:6789", 8989, true);
    RC_DEBUG << ad2.to_string();

    InetAddress ad3(6328, true, true);
    RC_DEBUG << ad3.to_string();

    return 0;
}