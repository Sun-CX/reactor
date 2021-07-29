//
// Created by suncx on 2020/8/23.
//

#ifndef REACTOR_CONNECTOR_H
#define REACTOR_CONNECTOR_H

#include "InetAddress.h"
#include "EventLoop.h"

namespace reactor::net {
    using reactor::net::Task;
    using std::enable_shared_from_this;

    class EventLoop;

    class Channel;

    class Connector final : public NonCopyable, public enable_shared_from_this<Connector> {
    private:
        using NewConnectionHandler = function<void(int)>;

        enum STATUS {
            DISCONNECTED, CONNECTING, CONNECTED
        };

        EventLoop *const loop;
        InetAddress server_addr;
        STATUS status;
        unique_ptr<Channel> channel;
        NewConnectionHandler con_handler;
        Task cur_task;
        // whether connector needs retry if connecting error.
        bool enable_retry;

        void start_in_loop();

        void stop_in_loop();

        void connect();

        void connecting(int fd);

        void handle_write();

        void handle_close();

        void handle_error();

        void remove_and_reset_channel();

        void reset_channel();

        bool is_self_connect(int fd) const;

        int get_sock_err(int sock) const;

        void retry(int fd);

        void close(int fd) const;

    public:
        Connector(EventLoop *loop, const InetAddress &addr);

        ~Connector();

        void on_connect(const NewConnectionHandler &handler);

        // cross-thread calling is allowed.
        void start();

        // cross-thread calling is allowed.
        void stop();
    };
}

#endif //REACTOR_CONNECTOR_H
