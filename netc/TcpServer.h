//
// Created by suncx on 2020/8/19.
//

#ifndef REACTOR_TCPSERVER_H
#define REACTOR_TCPSERVER_H

#include "NonCopyable.h"
#include "Events.h"
#include <string>
#include <map>
#include <atomic>

using std::string;
using std::map;
using std::atomic_int;
using std::unique_ptr;

class EventLoop;

class Acceptor;

class TcpConnection;

class EventLoopThreadPool;

class InetAddress;

class TcpServer final : public NonCopyable {
private:
    using ConnectionMap = map<string, shared_ptr<TcpConnection>>;
    using ThreadInitCallback = function<void(EventLoop *)>;

    EventLoop *loop;
    const string name;
    const string ip_port;
    unique_ptr<Acceptor> acceptor;
    shared_ptr<EventLoopThreadPool> thread_pool;
    atomic_int started;
    int next_conn_id;
    ConnectionMap connections;

    ConnectionCallback conn_callback;
    MessageCallback msg_callback;
    WriteCompleteCallback write_complete_callback;
    ThreadInitCallback thread_init_callback;

    void new_connection(int fd, const InetAddress &peer);

    void remove_connection(const shared_ptr<TcpConnection> &con);

public:
    TcpServer(EventLoop *loop, const InetAddress &listen_addr, string name, bool reuse_port = false);

};

#endif //REACTOR_TCPSERVER_H
