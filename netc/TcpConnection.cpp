//
// Created by suncx on 2020/8/19.
//

#include "TcpConnection.h"
#include "Ext.h"
#include "EventLoop.h"
#include "Timestamp.h"
#include "Socket.h"
#include "Channel.h"
#include "ConsoleStream.h"
#include <unistd.h>
#include <cassert>
#include <cstring>

using std::bind;
using reactor::net::TcpConnection;
using reactor::net::EventLoop;
using reactor::net::InetAddress;
using reactor::net::Buffer;
using std::any;

const char *TcpConnection::STATUS_STRING[4] = {"CONNECTING", "CONNECTED", "DISCONNECTING", "DISCONNECTED"};

TcpConnection::TcpConnection(EventLoop *loop, int con_fd, const InetAddress &local, const InetAddress &peer) :
        loop(loop),
        status(CONNECTING),
        reading(true),
        socket(new Socket(con_fd)),
        conn_channel(new Channel(loop, socket->get_fd())),
        local(local),
        peer(peer),
        inbound(),
        outbound(),
        context() {
    RC_DEBUG << "---------------------- +TcpConnection(" << conn_channel->get_fd() << ") ----------------------";
    socket->keep_alive(true);
    conn_channel->set_read_callback(bind(&TcpConnection::read_handler, this));
    conn_channel->set_write_callback(bind(&TcpConnection::write_handler, this));
    conn_channel->set_close_callback(bind(&TcpConnection::close_handler, this));
    conn_channel->set_error_callback(bind(&TcpConnection::error_handler, this));
}

TcpConnection::~TcpConnection() {
    assert(status == DISCONNECTED);
    RC_DEBUG << "---------------------- -TcpConnection(" << conn_channel->get_fd() << ") ----------------------";
}

void TcpConnection::read_handler() {
    assert(loop->is_in_created_thread());
    int saved_errno;
    ssize_t n = inbound.read_from_fd(conn_channel->get_fd(), saved_errno);
    if (n > 0) {
        // TODO: fix timestamp.
        msg_callback(shared_from_this(), Timestamp());
    } else if (n == 0) {

        RC_DEBUG << "read(" << conn_channel->get_fd() << ") 0 occurred!";

        // peer close will trigger close_handler(), so not call it here.

    } else {

        // peer error will triggered error_handler(), so not call it here.
        RC_ERROR << "read(" << conn_channel->get_fd() << ") error: " << ::strerror(saved_errno);
    }
}

void TcpConnection::write_handler() {
    assert(loop->is_in_created_thread());
    assert(outbound.readable_bytes() > 0);
    ssize_t n = ::write(conn_channel->get_fd(), outbound.peek(), outbound.readable_bytes());
    if (n > 0) {
        outbound.retrieve(n);
        if (outbound.readable_bytes() == 0) {
            conn_channel->disable_writing();

            if (!conn_channel->reading_enabled()) {
                assert(conn_channel->is_disabled());
            }

            if (write_complete_callback)
                loop->queue_in_loop(bind(write_complete_callback, shared_from_this()));
        }
    } else
        RC_ERROR << "write fd(" << conn_channel->get_fd() << ") error: " << ::strerror(errno);
}

void TcpConnection::close_handler() {
    assert(loop->is_in_created_thread());
    // 当 peer 主动断开连接时，状态为 CONNECTED
    // 当 host 主动断开连接时，状态为 DISCONNECTING

    RC_DEBUG << "con_fd " << conn_channel->get_fd() << " is closing, current status: " << STATUS_STRING[status];
    assert(status == CONNECTED or status == DISCONNECTING);

    if (status == CONNECTED)
        status = DISCONNECTING;

    // there is data left in out buffer to be sent.
    if (conn_channel->writing_enabled()) {
        conn_channel->disable_reading();
    } else {
        // no remaining data to be sent, going to close connection.
        conn_channel->remove();
        close_callback(shared_from_this());
    }
}

void TcpConnection::error_handler() {
    assert(loop->is_in_created_thread());
    int opt;
    socklen_t len = sizeof(opt);
    int ret = ::getsockopt(conn_channel->get_fd(), SOL_SOCKET, SO_ERROR, &opt, &len);
    if (unlikely(ret < 0))
        RC_FATAL << "getsockopt(" << conn_channel->get_fd() << ") error: " << ::strerror(errno);

    // error occurs.
    if (likely(opt))
        RC_ERROR << "connection(" << conn_channel->get_fd() << ") error: " << ::strerror(opt);

    conn_channel->remove();
    close_callback(shared_from_this());
}

void TcpConnection::connection_established() {
    assert(loop->is_in_created_thread());
    assert(status == CONNECTING);

    conn_callback(shared_from_this());
    status = CONNECTED;
    conn_channel->enable_reading();
}

void TcpConnection::connection_destroyed() {
    assert(loop->is_in_created_thread());
    assert(status == DISCONNECTING);
    status = DISCONNECTED;
    RC_DEBUG << "connection destroyed";
}

void TcpConnection::send() {
    assert(loop->is_in_created_thread());
    assert(status == CONNECTED);
    if (outbound.readable_bytes() > 0)
        conn_channel->enable_writing();
}

void TcpConnection::send_and_shutdown() {
    send();
    if (conn_channel->writing_enabled()) {
        set_write_complete_callback([](const shared_ptr <TcpConnection> &con) {
            con->shutdown();
        });
    } else
        shutdown();
}

void TcpConnection::shutdown() {
    assert(loop->is_in_created_thread());
    assert(status == CONNECTED);
    RC_DEBUG << "-------------- shutdown --------------";
    assert(!conn_channel->writing_enabled());
    socket->shutdown_write();
    status = DISCONNECTING;
}

void TcpConnection::force_close() {
    assert(loop->is_in_created_thread());
    assert(status == CONNECTED);
    RC_INFO << "-------------- quit --------------";
    status = DISCONNECTING;
    conn_channel->remove();
    connection_destroyed();
}

EventLoop *TcpConnection::get_loop() const {
    return loop;
}

const InetAddress &TcpConnection::local_address() const {
    return local;
}

const InetAddress &TcpConnection::peer_address() const {
    return peer;
}

Buffer &TcpConnection::in() {
    return inbound;
}

Buffer &TcpConnection::out() {
    return outbound;
}

int TcpConnection::get_fd() const {
    return socket->get_fd();
}

const any &TcpConnection::get_context() const {
    return context;
}

void TcpConnection::set_context(const any &ctx) {
    context = ctx;
}

void TcpConnection::set_connection_callback(const ConnectionCallback &callback) {
    conn_callback = callback;
}

void TcpConnection::set_message_callback(const MessageCallback &callback) {
    msg_callback = callback;
}

void TcpConnection::set_write_complete_callback(const WriteCompleteCallback &callback) {
    write_complete_callback = callback;
}

void TcpConnection::set_close_callback(const CloseCallback &callback) {
    close_callback = callback;
}