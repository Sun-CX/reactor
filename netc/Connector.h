//
// Created by suncx on 2020/8/23.
//

#ifndef REACTOR_CONNECTOR_H
#define REACTOR_CONNECTOR_H

#include "NonCopyable.h"
#include "InetAddress.h"
#include <memory>
#include <functional>

namespace reactor::net {
    using reactor::core::NonCopyable;
    using std::enable_shared_from_this;
    using std::function;
    using std::unique_ptr;

    class EventLoop;

    class Channel;

    class Connector final : public NonCopyable, public enable_shared_from_this<Connector> {
    private:
        using NewConnectionCallback = function<void(int)>;

        enum STATUS {
            DISCONNECTED, CONNECTING, CONNECTED
        };

        EventLoop *loop;
        InetAddress server_addr;
        STATUS status;
        unique_ptr<Channel> channel;
        NewConnectionCallback connection_callback;
        int retry_delay_ms;

        void start_in_loop();

        void stop_in_loop();

        void connect();

        void connecting(int sock_fd);

        void handle_write();

        void handle_error();

        int remove_and_reset_channel();

        void reset_channel();

        bool is_self_connect(int fd) const;

        int get_sock_err(int sock_fd) const;

        void retry(int sock_fd);

    public:
        Connector(EventLoop *loop, const InetAddress &addr);

        ~Connector();

        void set_new_conn_callback(const NewConnectionCallback &callback);

        void start();

//        void restart();

        void stop();

        const InetAddress &get_server_addr() const;
    };
}

#endif //REACTOR_CONNECTOR_H
