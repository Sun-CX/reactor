//
// Created by suncx on 2020/8/19.
//

#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"
#include "ConsoleStream.h"
#include <cassert>
#include <sys/resource.h>
#include <cstring>

using std::shared_ptr;
using std::make_shared;
using std::bind;
using std::to_string;
using std::placeholders::_1;
using std::placeholders::_2;
using std::chrono::system_clock;
using reactor::net::TcpConnection;
using reactor::net::TcpServer;
using reactor::core::Timestamp;

static void default_connection_handler(const shared_ptr<TcpConnection> &conn) {
    RC_INFO << "New Connection: " << conn->peer_address().to_string()
            << " ----------> " << conn->local_address().to_string();
}

static void default_data_handler(const shared_ptr<TcpConnection> &conn, Timestamp ts) {
    conn->in().retrieve_all();
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &bind_addr, string name, int threads, bool reuse_port)
        : loop(loop),
          name(move(name)),
          acceptor(new Acceptor(loop, bind_addr, reuse_port)),
          thread_pool(new EventLoopThreadPool(loop, this->name, threads)),
          connections(),
          connection_handler(default_connection_handler),
          data_handler(default_data_handler) {
    acceptor->on_new_connection(bind(&TcpServer::on_new_connection, this, _1, _2));

    rlimit lim;
    if (::getrlimit(RLIMIT_NOFILE, &lim) < 0)
        RC_FATAL << "getrlimit error: " << ::strerror(errno);

    lim.rlim_cur = lim.rlim_max;

    if (::setrlimit(RLIMIT_NOFILE, &lim) < 0)
        RC_FATAL << "setrlimit error: " << ::strerror(errno);
}

TcpServer::~TcpServer() {
    assert(loop->is_in_created_thread());
    RC_INFO << "---------------------- ~TcpServer ----------------------";
    for (auto &e : connections) {
        shared_ptr<TcpConnection> conn(e.second);
//        INFO << "conn use count: " << conn.use_count();
        e.second.reset();
//        INFO << "conn use count: " << conn.use_count();
        assert(e.second == nullptr);
        conn->get_loop()->queue_in_loop(bind(&TcpConnection::force_close, conn));
    }
}

void TcpServer::on_new_connection(int con_fd, const InetAddress &peer) {
    assert(loop->is_in_created_thread());

    EventLoop *io_loop = thread_pool->get_next_loop();

    InetAddress local = InetAddress::get_local_address(con_fd);

    auto conn = make_shared<TcpConnection>(io_loop, con_fd, local, peer);
    conn->on_connection(connection_handler);
    conn->on_data(data_handler);
    conn->on_write_complete(write_complete_handler);
    conn->on_close(bind(&TcpServer::remove_connection, this, _1));
    connections[con_fd] = conn;

    io_loop->run_in_loop(bind(&TcpConnection::connection_established, conn));
}

void TcpServer::remove_connection(const shared_ptr<TcpConnection> &con) {
    loop->run_in_loop(bind(&TcpServer::remove_connection_in_loop, this, con));
}

void TcpServer::remove_connection_in_loop(const shared_ptr<TcpConnection> &con) {
    assert(loop->is_in_created_thread());
    auto n = connections.erase(con->get_fd());
    assert(n == 1);
    con->get_loop()->run_in_loop(bind(&TcpConnection::connection_destroyed, con));
}

void TcpServer::start() {
    assert(loop->is_in_created_thread());
    assert(!acceptor->is_listening());
    thread_pool->start(thread_initial_callback);
    acceptor->listen();
}

void TcpServer::on_connection(const ConnectionHandler &handler) {
    connection_handler = handler;
}

void TcpServer::on_data(const DataHandler &handler) {
    data_handler = handler;
}

void TcpServer::on_write_complete(const WriteCompleteHandler &handler) {
    write_complete_handler = handler;
}

void TcpServer::set_thread_initial_callback(const decltype(thread_initial_callback) &callback) {
    thread_initial_callback = callback;
}