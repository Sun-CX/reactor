//
// Created by suncx on 2020/8/19.
//

#include "TcpConnection.h"
#include "Exception.h"
#include "EventLoop.h"
#include "Timestamp.h"
#include "Socket.h"
#include "Channel.h"
#include "ConsoleStream.h"
#include <unistd.h>
#include <cassert>

using std::bind;

const char *TcpConnection::STATUS_STR[4] = {"Connecting", "Connected", "Disconnecting", "Disconnected"};

TcpConnection::TcpConnection(EventLoop *loop, string name, int con_fd, const InetAddress &local,
                             const InetAddress &peer) :
        loop(loop), name(move(name)), status(Connecting), reading(true), socket(new Socket(con_fd)),
        conn_channel(new Channel(loop, socket->get_fd())), local(local), peer(peer),
        high_water_mark(64 * 1024 * 1024), context(nullptr) {
    socket->keep_alive(true);
    conn_channel->set_read_callback(bind(&TcpConnection::read_handler, this));
    conn_channel->set_write_callback(bind(&TcpConnection::write_handler, this));
    conn_channel->set_close_callback(bind(&TcpConnection::close_handler, this));
    conn_channel->set_error_callback(bind(&TcpConnection::error_handler, this));
}

void TcpConnection::read_handler() {
    assert(loop->is_in_loop_thread());
    int saved_errno = 0;
    ssize_t n = inbound.read_from_fd(conn_channel->get_fd(), &saved_errno);
    if (n > 0) {
        //TODO:fix timestamp.
        msg_callback(shared_from_this(), Timestamp());
    } else if (n == 0) {
        LOG << "read 0 bytes from con_fd " << conn_channel->get_fd() << ", prepare to close connection.";
        close_handler();
    } else {
        errno = saved_errno;
        ERROR << "read from inbound buffer error, prepare to close connection.";
        error_handler();
    }
}

void TcpConnection::write_handler() {
    assert(loop->is_in_loop_thread());
    auto n = write(conn_channel->get_fd(), outbound.peek(), outbound.readable_bytes());
    if (n > 0) {
        outbound.retrieve(n);
        if (outbound.readable_bytes() == 0) {
            conn_channel->disable_writing();
            if (write_complete_callback) {
                loop->queue_in_loop(bind(write_complete_callback, shared_from_this()));
            }
            if (status == Disconnecting) {
                shutdown_in_loop();
            }
        }
    } else
        ERROR << "write outbound data error!";

}

void TcpConnection::close_handler() {
    LOG << "con_fd " << conn_channel->get_fd() << " is closing, current status: " << STATUS_STR[status];
    assert(loop->is_in_loop_thread());
    // 当 peer 主动断开连接时，状态为 Connected
    // 当 host 主动断开连接时，状态为 Disconnecting
    assert(status == Connected or status == Disconnecting);
    if (status == Connected) status = Disconnecting;
    conn_channel->disable_all();
    conn_channel->remove();
    close_callback(shared_from_this());
}

void TcpConnection::error_handler() {
    int opt;
    socklen_t len = sizeof(opt);
    auto n = getsockopt(conn_channel->get_fd(), SOL_SOCKET, SO_ERROR, &opt, &len);
    if (unlikely(n < 0)) ERROR << "getsockopt error! errno = " << errno;
    else
        ERROR << "TcpConnection::error_handler! errno: " << opt;
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

void TcpConnection::connection_established() {
    assert(loop->is_in_loop_thread() and status == Connecting);
    status = Connected;
    conn_channel->enable_reading();
    conn_callback(shared_from_this());
}

void TcpConnection::connection_destroyed() {
    assert(loop->is_in_loop_thread());
    assert(status == Disconnected);
    LOG << "connection disconnected.";
}

void TcpConnection::shutdown_in_loop() {
    assert(loop->is_in_loop_thread());
    if (!conn_channel->writing_enabled()) socket->shutdown_write();
}

TcpConnection::~TcpConnection() {
    assert(status == Disconnected);
}

void TcpConnection::send_outbound_bytes() {
    LOG << "-------------- send_outbound_bytes --------------";
    LOG << "current status: " << STATUS_STR[status];
    assert(loop->is_in_loop_thread() and status == Connected);
    conn_channel->enable_writing();
}

void TcpConnection::shutdown() {
    assert(status == Connected);
    LOG << "-------------- shutdown --------------";
    LOG << "current status: " << STATUS_STR[status];
    if (status == Connected) {
        status = Disconnecting;
        loop->run_in_loop(bind(&TcpConnection::shutdown_in_loop, this));
    }
}

EventLoop *TcpConnection::get_loop() const {
    return loop;
}

const string &TcpConnection::get_name() const {
    return name;
}

const InetAddress &TcpConnection::local_address() const {
    return local;
}

const InetAddress &TcpConnection::peer_address() const {
    return peer;
}

bool TcpConnection::connected() const {
    return status == Connected;
}

Buffer &TcpConnection::inbound_buf() {
    return inbound;
}

Buffer &TcpConnection::outbound_buf() {
    return outbound;
}

void TcpConnection::set_status(STATUS state) {
    status = state;
}

TcpConnection::STATUS TcpConnection::get_status() const {
    return status;
}

const any &TcpConnection::get_context() const {
    return context;
}

void TcpConnection::set_context(const any &ctx) {
    context = ctx;
}
