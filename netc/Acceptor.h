//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_ACCEPTOR_H
#define REACTOR_ACCEPTOR_H

#include "ServerSocket.h"
#include "Channel.h"

namespace reactor::net {
    using reactor::core::NonCopyable;

    class EventLoop;

    class Acceptor final : public NonCopyable {
    private:
        using NewConnectionCallback = function<void(int, const InetAddress &)>;

        EventLoop *const loop;
        ServerSocket server_socket;
        Channel accept_channel;
        NewConnectionCallback callback;
        bool listening;
        int idle_fd;

        void read_handler();

        [[nodiscard]]
        int open_idle_fd() const;

        void close_idle_fd() const;

    public:
        Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port);

        ~Acceptor();

        void set_new_connection_callback(const NewConnectionCallback &handler);

        void listen();

        [[nodiscard]]
        bool is_listening() const;
    };
}

#endif //REACTOR_ACCEPTOR_H