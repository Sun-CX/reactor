//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_SERVERSOCKET_H
#define REACTOR_SERVERSOCKET_H

#include "NonCopyable.h"

namespace reactor::net {
    using reactor::core::NonCopyable;

    class InetAddress;

    class ServerSocket final : public NonCopyable {
    private:
        const int listen_fd;  // listen_fd 的生存期由 ServerSocket 类控制，在 ServerSocket 析构时会将其关闭

        [[nodiscard]]
        int create_socket() const;

    public:
        // only open a IPv4 socket right now.
        ServerSocket();

        ~ServerSocket();

        [[nodiscard]]
        int fd() const;

        void bind(const InetAddress &addr) const;

        void listen() const;

        // On success, this returns a nonnegative integer that is a file descriptor for the accepted socket.
        // On error, -1 is returned, and errno is set appropriately.
        int accept(InetAddress &peer_addr) const;

        // enable or disable SO_REUSEADDR option on a socket.
        // SO_REUSEADDR option is disabled by default.
        void reuse_addr(bool on) const;

        // enable or disable SO_REUSEPORT option on a socket.
        // SO_REUSEPORT option is disabled by default.
        void reuse_port(bool on) const;

        // enable or disable TCP_NODELAY option on a socket.
        // TCP_NODELAY option is disabled by default.
        void no_delay(bool on) const;
    };
}

#endif //REACTOR_SERVERSOCKET_H
