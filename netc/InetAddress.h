//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_INETADDRESS_H
#define REACTOR_INETADDRESS_H

#include <netinet/in.h>
#include <string>

using std::string;

/**
 * 暂时只支持 IPv4
 */
class InetAddress final {
private:
    sockaddr_in ad4;    // 以网络序（大端序）存储
//        sockaddr_in6 ad6;

    friend string to_readable_string(const InetAddress &addr);

public:

    /**
     * 描述网络地址
     * @param ip 网络 IP，如 "127.0.0.1"
     * @param port 端口号
     */
    InetAddress();

    explicit InetAddress(const char *ip, uint16_t port);

    explicit InetAddress(bool use_loop_back, uint16_t port);

    const sockaddr *get_sockaddr() const;

    /**
     * 设置网络序地址
     * @param addr 网络序地址
     */
    void set_sockaddr(const sockaddr_in &addr);

    static InetAddress get_local_address(int fd);
};

#endif //REACTOR_INETADDRESS_H
