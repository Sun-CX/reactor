//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_SERVERSOCKET_H
#define REACTOR_SERVERSOCKET_H

#include "NonCopyable.h"

class InetAddress;

// 封装监听套接字
class ServerSocket final : public NonCopyable {
private:
    const int sock_fd;  // sock_fd 的生存期由 ServerSocket 类控制，在 ServerSocket 析构时会将其关闭

    int create_socket() const;

public:
    ServerSocket();

    virtual ~ServerSocket();

    int fd() const;

    void bind(const InetAddress &addr) const;

    void listen() const;

    /**
     * 接受客户端的连接，返回已连接套接字：
     *
     * 当连接成功时，返回已连接套接字，并设置对端的地址
     *
     * 连接失败返回 -1， errno 保存对应的错误码
     * @param peer_addr 对端地址
     * @return 已连接套接字
     */
    int accept(InetAddress *peer_addr) const;

    void tcp_no_delay(bool on) const;

    void reuse_addr(bool on) const;

    void reuse_port(bool on) const;

    void keep_alive(bool on) const;
};

#endif //REACTOR_SERVERSOCKET_H
