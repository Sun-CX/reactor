//
// Created by suncx on 2020/8/19.
//

#include "TcpServer.h"
#include "Timestamp.h"
#include "Buffer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"
#include <cassert>

using std::make_shared;
using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

// 如果客户端代码没有设置连接回调，则调用此默认连接回调
void default_connection_callback(const shared_ptr<TcpConnection> &conn) {
    printf("%s[%d]: new client connected: %s <------------------- %s\n", CurrentThread::name, CurrentThread::pid,
           conn->local_address().to_string().c_str(),
           conn->peer_address().to_string().c_str());
}

// 如果客户端代码没有设置消息到来回调，则调用此默认消息回调
void default_message_callback(const shared_ptr<TcpConnection> &conn, Buffer *buf, Timestamp s) {
    buf->retrieve_all();
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &bind_addr, string name, bool reuse_port)
        : loop(loop), name(move(name)), ip_port(bind_addr.to_string()),
          acceptor(new Acceptor(loop, bind_addr, reuse_port)),
          thread_pool(new EventLoopThreadPool(loop, 5, this->name)),
          conn_callback(default_connection_callback), msg_callback(default_message_callback), started(0),
          next_conn_id(1) {
    acceptor->set_new_connection_callback(bind(&TcpServer::on_new_connection, this, _1, _2));
}

TcpServer::~TcpServer() {
    assert(loop->is_in_loop_thread());
    for (auto &e:connections) {
        shared_ptr<TcpConnection> conn(e.second);
        e.second.reset();
        conn->get_loop()->run_in_loop(bind(&TcpConnection::connection_destroyed, conn));
    }
}

// 一旦新的连接到来，则构造 TcpConnection 对象，存入 connections 中
void TcpServer::on_new_connection(int con_fd, const InetAddress &peer) {
    assert(loop->is_in_loop_thread());
    auto io_loop = thread_pool->get_next_loop();
    char buf[32];
    snprintf(buf, sizeof(buf), "-%s-%d", ip_port.c_str(), next_conn_id++);

    string conn_name = name + buf;
    InetAddress local = InetAddress::get_local_address(con_fd);

    shared_ptr<TcpConnection> conn = make_shared<TcpConnection>(io_loop, conn_name, con_fd, local, peer);

    conn->set_connection_callback(conn_callback);
    conn->set_message_callback(msg_callback);
    conn->set_write_complete_callback(write_complete_callback);
    conn->set_close_callback(bind(&TcpServer::remove_connection, this, _1));
    connections[conn_name] = conn;
    io_loop->run_in_loop(bind(&TcpConnection::connection_established, conn));
}

void TcpServer::remove_connection(const shared_ptr<TcpConnection> &con) {
    loop->run_in_loop(bind(&TcpServer::remove_connection_in_loop, this, con));
}

void TcpServer::remove_connection_in_loop(const shared_ptr<TcpConnection> &con) {
    assert(loop->is_in_loop_thread());
    auto n = connections.erase(con->get_name());
    assert(n == 1);
    EventLoop *io_loop = con->get_loop();
    io_loop->queue_in_loop(bind(&TcpConnection::connection_destroyed, con));
}

void TcpServer::start() {
    thread_pool->start(thread_init_callback);
    assert(not acceptor->is_listening());
    loop->run_in_loop(bind(&Acceptor::listen, acceptor.get()));
}

void TcpServer::set_conn_callback(const ConnectionCallback &callback) {
    conn_callback = callback;
}

void TcpServer::set_msg_callback(const MessageCallback &callback) {
    msg_callback = callback;
}

void TcpServer::set_write_complete_callback(const WriteCompleteCallback &callback) {
    write_complete_callback = callback;
}

void TcpServer::set_thread_initial_callback(const ThreadInitCallback &callback) {
    thread_init_callback = callback;
}
