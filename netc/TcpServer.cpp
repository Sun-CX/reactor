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
#include "EventLoopThreadPool.h"

using std::make_shared;
using std::placeholders::_1;
using std::placeholders::_2;

void default_connection_callback(const shared_ptr<TcpConnection> &conn) {
    printf("hello world\n");
}

void default_message_callback(const shared_ptr<TcpConnection> &conn, Buffer *buf, Timestamp s) {
    buf->retrieve_all();
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listen_addr, string name, bool reuse_port)
        : loop(loop), name(move(name)), ip_port(to_readable_string(listen_addr)),
          acceptor(new Acceptor(loop, listen_addr, reuse_port)),
          thread_pool(new EventLoopThreadPool(loop, move(name))),
          conn_callback(default_connection_callback),
          msg_callback(default_message_callback),
          next_conn_id(1) {
    acceptor->set_read_handler(bind(&TcpServer::new_connection, this, _1, _2));

}

void TcpServer::new_connection(int fd, const InetAddress &peer) {
    loop->assert_in_created_thread();
    auto io_loop = thread_pool->get_next_loop();
    char buf[64];
    snprintf(buf, sizeof(buf), "-%s-%d", ip_port.c_str(), next_conn_id++);

    string conn_name = name + buf;
    InetAddress local = InetAddress::get_local_address(fd);

    shared_ptr<TcpConnection> conn = make_shared<TcpConnection>(io_loop, conn_name, fd, local, peer);

    connections[conn_name] = conn;
    conn->set_connection_callback(conn_callback);
    conn->set_message_callback(msg_callback);
    conn->set_write_complete_callback(write_complete_callback);
    conn->set_close_callback(bind(&TcpServer::remove_connection, this, _1));
    io_loop->run_in_loop(bind(&TcpConnection::connection_established, conn));
}
