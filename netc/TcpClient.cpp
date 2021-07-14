//
// Created by 孙诚雄 on 2021/7/7.
//

#include "TcpClient.h"
#include "EventLoop.h"
#include "Connector.h"
#include "TcpConnection.h"
#include <cassert>

using reactor::net::TcpClient;
using std::placeholders::_1;
using reactor::core::MutexGuard;
using std::make_shared;

TcpClient::TcpClient(EventLoop *loop, const InetAddress &addr, string name) :
        loop(loop),
        connector(new Connector(this->loop, addr)),
        name(move(name)),
        connection_callback(),
        message_callback(),
        write_complete_callback(),
        retry(false),
        next_con_id(1),
        mutex(),
        connection() {
    connector->set_new_conn_callback(bind(&TcpClient::new_connection, this, _1));
}

TcpClient::~TcpClient() {
    shared_ptr <TcpConnection> con;
    bool unique;
    {
        MutexGuard guard(mutex);
        unique = connection.unique();
        con = connection;
    }

    if (con) {
        assert(loop == con->get_loop());


    }

}

void TcpClient::connect() {
    connector->start();
}

void TcpClient::stop() {
    connector->stop();
}

void TcpClient::disconnect() {
    MutexGuard guard(mutex);
    if (connection) {
        connection->shutdown();
    }
}

void TcpClient::new_connection(int sock_fd) {
    assert(loop->is_in_loop_thread());

    InetAddress peer = InetAddress::get_peer_address(sock_fd);
    InetAddress local = InetAddress::get_local_address(sock_fd);
    shared_ptr <TcpConnection> conn = make_shared<TcpConnection>(loop, sock_fd, local, peer);

    conn->set_connection_callback();
    conn->set_message_callback();
    conn->set_write_complete_callback();
    conn->set_close_callback();

    conn->connection_established();
}
