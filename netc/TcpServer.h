//
// Created by suncx on 2020/8/19.
//

#ifndef REACTOR_TCPSERVER_H
#define REACTOR_TCPSERVER_H

#include "Handlers.h"
#include "EventLoopThread.h"
#include <map>

namespace reactor::net {
    using std::string;
    using std::map;
    using std::unique_ptr;

    class EventLoop;

    class Acceptor;

    class TcpConnection;

    class EventLoopThreadPool;

    class InetAddress;

    class TcpServer final : public NonCopyable {
    private:
        using ConnectionMap = map<int, shared_ptr<TcpConnection>>;

        EventLoop *const loop;
        const string name;
        unique_ptr<Acceptor> acceptor;
        shared_ptr<EventLoopThreadPool> thread_pool;
        ConnectionMap connections;

        ConnectionHandler con_handler;
        DataHandler data_handler;
        WriteCompleteHandler write_complete_handler;
        EventLoopThread::ThreadInitializer thread_initial_callback;

        void on_new_connection(int con_fd, const InetAddress &peer);

        void remove_connection(const shared_ptr<TcpConnection> &con);

        void remove_connection_in_loop(const shared_ptr<TcpConnection> &con);

    public:
        TcpServer(EventLoop *loop, const InetAddress &bind_addr, string name, int threads, bool reuse_port = false);

        ~TcpServer();

        void start();

        // `handler` will be called when a new client connection is established.
        void on_connect(const ConnectionHandler &handler);

        // `handler` will be called when data from client is received.
        void on_data(const DataHandler &handler);

        // `handler` will be called when data from server is completely sent.
        void on_write_complete(const WriteCompleteHandler &handler);

        void set_thread_initial_callback(const decltype(thread_initial_callback) &callback);
    };
}

#endif //REACTOR_TCPSERVER_H
