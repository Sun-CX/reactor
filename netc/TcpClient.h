//
// Created by 孙诚雄 on 2021/7/7.
//

#ifndef REACTOR_TCPCLIENT_H
#define REACTOR_TCPCLIENT_H

#include <memory>
#include "Handlers.h"
#include "Mutex.h"

namespace reactor::net {
    using reactor::core::NonCopyable;
    using reactor::core::Mutex;
    using std::shared_ptr;
    using std::string;

    class TcpConnection;

    class EventLoop;

    class Connector;

    class InetAddress;

    class TcpClient final : public NonCopyable {
    private:
        EventLoop *loop;
        shared_ptr<Connector> connector;
        string name;
        ConnectionCallback connection_callback;
        MessageCallback message_callback;
        WriteCompleteCallback write_complete_callback;
        bool retry;
        int next_con_id;
        mutable Mutex mutex;
        shared_ptr<TcpConnection> connection;

        void new_connection(int sock_fd);

        void remove_connection(const shared_ptr<TcpConnection> &con);

    public:
        TcpClient(EventLoop *loop, const InetAddress &addr, string name);

        ~TcpClient();

        void connect();

        void disconnect();

        void stop();
    };
}

#endif //REACTOR_TCPCLIENT_H
