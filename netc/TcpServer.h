//
// Created by suncx on 2020/8/19.
//

#ifndef REACTOR_TCPSERVER_H
#define REACTOR_TCPSERVER_H

#include "Events.h"
#include "EventLoopThread.h"
#include <string>
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

        ConnectionCallback new_connection_callback;
        MessageCallback message_callback;
        WriteCompleteCallback write_complete_callback;
        EventLoopThread::ThreadInitializer thread_initial_callback;

        void on_new_connection(int con_fd, const InetAddress &peer);

        void remove_connection(const shared_ptr<TcpConnection> &con);

        void remove_connection_in_loop(const shared_ptr<TcpConnection> &con);

    public:
        TcpServer(EventLoop *loop, const InetAddress &bind_addr, string name, int threads, bool reuse_port = false);

        ~TcpServer();

        void start();

        void set_new_connection_callback(const ConnectionCallback &callback);

        void set_message_callback(const MessageCallback &callback);

        void set_write_complete_callback(const WriteCompleteCallback &callback);

        void set_thread_initial_callback(const decltype(thread_initial_callback) &callback);
    };
}

#endif //REACTOR_TCPSERVER_H
