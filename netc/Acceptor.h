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
        using NewConnectionHandler = function<void(int, const InetAddress &)>;

        EventLoop *const loop;
        ServerSocket server_socket;
        Channel accept_channel;
        NewConnectionHandler handler;
        bool listening;
        int idle_fd;

        void handle_read(Timestamp ts);

        [[nodiscard]]
        int open_idle_fd() const;

        void close_idle_fd() const;

    public:
        Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port);

        ~Acceptor();

        void on_connect(const NewConnectionHandler &hdr);

        void listen();

        [[nodiscard]]
        bool is_listening() const;
    };
}

#endif //REACTOR_ACCEPTOR_H