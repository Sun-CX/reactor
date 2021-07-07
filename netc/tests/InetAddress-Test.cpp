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

void ipv4() {
    InetAddress addr(9090, true);
    RC_DEBUG << addr.to_string();

    InetAddress ad("192.168.2.2", 8080);
    RC_DEBUG << ad.to_string();
    RC_DEBUG << ad.ip_string();
}

void ipv6() {

    InetAddress ad2("ABCD:EF01:2345:6789:ABCD:EF01:2345:6789", 8989, AF_INET6);
    RC_DEBUG << ad2.to_string();
    RC_DEBUG << ad2.ip_string();

    InetAddress ad3(6328, true, AF_INET6);
    RC_DEBUG << ad3.to_string();

    InetAddress ad4("0:0:0:0:0:0:0:0", 12000, AF_INET6);
    RC_DEBUG << ad4.to_string();

    InetAddress ad5("1:0:0:0:0:0:0:8", 14000, AF_INET6);
    RC_DEBUG << ad5.to_string();

    InetAddress ad6("0:0:0:0:0:FFFF:204.152.189.116", 16000, AF_INET6);
    RC_DEBUG << ad6.to_string();
}

void resolve_test() {
    InetAddress ad4 = InetAddress::resolve("baidu.com");
    RC_DEBUG << ad4.ip_string();

    InetAddress ad5 = InetAddress::resolve("zhihu.com");
    RC_DEBUG << ad5.ip_string();
}

int main(int argc, const char *argv[]) {

    ipv4();
    ipv6();
    resolve_test();

    return 0;
}