//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_INETADDRESS_H
#define REACTOR_INETADDRESS_H

#include <netinet/in.h>
#include <string>

namespace reactor::net {
    using std::string;

// 对网络地址的封装（目前只支持 IPv4）
    class InetAddress final {
    private:
        sockaddr_in ad4;    // 以网络序（大端序）存储
    public:
        InetAddress();

        explicit InetAddress(const char *ip, uint16_t port);

        explicit InetAddress(bool use_loop_back, uint16_t port);

        [[nodiscard]]
        const sockaddr *get_sockaddr() const;

        // 设置网络序地址
        void set_sockaddr(const sockaddr_in &addr);

        // 返回 ip:port 格式的字符串
        [[nodiscard]]
        string to_string() const;

        static InetAddress get_local_address(int fd);
    };
}

#endif //REACTOR_INETADDRESS_H
