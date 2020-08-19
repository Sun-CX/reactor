//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_SOCKET_H
#define REACTOR_SOCKET_H

#include "NonCopyable.h"

class InetAddress;

class Socket final : public NonCopyable {
private:
    int sock_fd;
public:
    explicit Socket(int sock_fd);

    virtual ~Socket();

    int fd() const;

    void bind(const InetAddress &addr);

    void listen();

    int accept(InetAddress *peer_addr);

    void tcp_no_delay(bool on) const;

    void reuse_addr(bool on) const;

    void reuse_port(bool on) const;

    void keep_alive(bool on) const;
};


#endif //REACTOR_SOCKET_H
