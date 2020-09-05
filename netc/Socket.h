//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_SOCKET_H
#define REACTOR_SOCKET_H

#include "NonCopyable.h"

class InetAddress;

class Socket final : public NonCopyable {
private:
    const int sock_fd;  // fd 的生存期由 Socket 类控制，在 Socket 析构时会将其关闭
    int create_socket() const;

public:
    Socket();

    virtual ~Socket();

    int fd() const;

    void bind(const InetAddress &addr);

    void listen();

    /**
     * 接受客户端的连接，返回已连接套接字：
     *
     * 当连接成功时，返回已连接套接字，并设置对端的地址
     *
     * 连接失败返回 -1， errno 保存对应的错误码
     * @param peer_addr 对端地址
     * @return 已连接套接字
     */
    int accept(InetAddress *peer_addr);

    void tcp_no_delay(bool on) const;

    void reuse_addr(bool on) const;

    void reuse_port(bool on) const;

    void keep_alive(bool on) const;
};

#endif //REACTOR_SOCKET_H
