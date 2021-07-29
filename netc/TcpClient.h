//
// Created by 孙诚雄 on 2021/7/7.
//

#ifndef REACTOR_TCPCLIENT_H
#define REACTOR_TCPCLIENT_H

#include "Mutex.h"
#include "Handlers.h"

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
        EventLoop *const loop;
        shared_ptr<Connector> connector;
        string name;
        ConnectionHandler con_handler;
        DataHandler data_handler;
        WriteCompleteHandler write_complete_handler;
        bool retry;
        int next_con_id;
        mutable Mutex mutex;
        shared_ptr<TcpConnection> con_holder;

        void new_connection(int fd);

        void remove_connection(const shared_ptr<TcpConnection> &con);

    public:
        TcpClient(EventLoop *loop, const InetAddress &addr, string name);

        ~TcpClient();

        void connect();

        void disconnect();

        void stop();

        void on_connect(const ConnectionHandler &handler);

        void on_data(const DataHandler &handler);

        void on_write_complete(const WriteCompleteHandler &handler);
    };
}

#endif //REACTOR_TCPCLIENT_H
