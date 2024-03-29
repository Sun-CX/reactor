//
// Created by suncx on 2020/9/7.
//

#ifndef REACTOR_SOCKET_H
#define REACTOR_SOCKET_H

#include "NonCopyable.h"

namespace reactor::net {
    using reactor::core::NonCopyable;

    // 封装已连接套接字
    class Socket final : public NonCopyable {
    private:
        const int fd;
    public:
        explicit Socket(int con_fd);

        ~Socket();

        [[nodiscard]]
        int get_fd() const;

        void shutdown_read() const;

        void shutdown_write() const;

        void keep_alive(bool on) const;

        [[nodiscard]]
        int get_error() const;
    };
}

#endif //REACTOR_SOCKET_H
