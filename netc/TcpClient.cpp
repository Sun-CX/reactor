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
        con_handler(),
        data_handler(),
        write_complete_handler(),
        retry(false),
        next_con_id(1),
        mutex(),
        con_holder() {
    connector->on_connect(bind(&TcpClient::new_connection, this, _1));
}

TcpClient::~TcpClient() {
    shared_ptr <TcpConnection> con;
    bool unique = false;
    {
        MutexGuard guard(mutex);
        unique = con_holder.unique();
        con = con_holder;
    }

    if (con) {
        assert(loop == con->get_loop());
        con->force_close();
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
    if (con_holder) {
        con_holder->close();
    }
}

void TcpClient::new_connection(const int fd) {
    assert(loop->is_in_created_thread());

    InetAddress peer = InetAddress::get_peer_address(fd);
    InetAddress local = InetAddress::get_local_address(fd);
    auto con = make_shared<TcpConnection>(loop, fd, local, peer);

    con->on_connect(con_handler);
    con->on_data(data_handler);
    con->on_write_complete(write_complete_handler);
    con->on_close(bind(&TcpClient::remove_connection, this, _1));

    // hold this connection, otherwise is will be freed.
    {
        MutexGuard guard(mutex);
        con_holder = con;
    }

    con->connection_established();
}

void TcpClient::remove_connection(const shared_ptr <TcpConnection> &con) {
    assert(loop->is_in_created_thread());

    {
        MutexGuard guard(mutex);
        assert(con_holder == con);
        con_holder.reset();
        assert(con_holder == nullptr);
    }

    loop->queue_in_loop(bind(&TcpConnection::connection_destroyed, con));

    // TODO: 断线重连
}

void TcpClient::on_connect(const ConnectionHandler &handler) {
    con_handler = handler;
}

void TcpClient::on_data(const DataHandler &handler) {
    data_handler = handler;
}

void TcpClient::on_write_complete(const WriteCompleteHandler &handler) {
    write_complete_handler = handler;
}
