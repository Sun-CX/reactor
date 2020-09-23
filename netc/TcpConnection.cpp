//
// Created by suncx on 2020/8/19.
//

#include "TcpConnection.h"
#include "Exception.h"
#include "EventLoop.h"
#include "Timestamp.h"
#include "Socket.h"
#include "Channel.h"
#include "StringPiece.h"
#include <unistd.h>
#include <cassert>

using std::bind;

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
        printf("%s[%d]: read 0 from con_fd(%d).\n", CurrentThread::name, CurrentThread::pid, conn_channel->get_fd());
        close_handler();
    } else {
        errno = saved_errno;
        fprintf(stderr, "read_handler from TcpConnection error.");
        error_handler();
    }
}

void TcpConnection::write_handler() {
    assert(loop->is_in_loop_thread());
    if (conn_channel->writing_enabled()) {
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
        } else {
            fprintf(stderr, "write error.\n");
        }
    } else {
        printf("Connection fd: %d is down, no more writing...\n", conn_channel->get_fd());
    }
}

void TcpConnection::close_handler() {
    assert(loop->is_in_loop_thread());
    assert(status == Connected or status == Disconnecting);
    printf("%s[%d]: con_fd %d closed.\n", CurrentThread::name, CurrentThread::pid, conn_channel->get_fd());
    conn_channel->disable_all();
    conn_channel->remove();

    status = Disconnected;
    close_callback(shared_from_this());
}

void TcpConnection::error_handler() {
    int opt;
    socklen_t len = sizeof(opt);
    auto n = getsockopt(conn_channel->get_fd(), SOL_SOCKET, SO_ERROR, &opt, &len);
    if (unlikely(n < 0)) fprintf(stderr, "TcpConnection::error_handler, errno: %d\n", errno);
    else fprintf(stderr, "TcpConnection::error_handler, errno: %d\n", opt);
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

void TcpConnection::set_high_water_mark_callback(const HighWaterMarkCallback &callback) {
    high_water_mark_callback = callback;
}

void TcpConnection::set_close_callback(const CloseCallback &callback) {
    close_callback = callback;
}

void TcpConnection::connection_established() {
    assert(loop->is_in_loop_thread() and status == Connecting);
    status = Connected;
    //TODO:fix tie

    conn_channel->enable_reading();
    conn_callback(shared_from_this());
}

void TcpConnection::connection_destroyed() {
    assert(loop->is_in_loop_thread());
    assert(status == Disconnected);
    printf("connection disconnected finished.\n");
}

void TcpConnection::shutdown_in_loop() {
    assert(loop->is_in_loop_thread());
    if (!conn_channel->writing_enabled()) {
        socket->shutdown_write();
    }
}

TcpConnection::~TcpConnection() {
    assert(status == Disconnected);
}

void TcpConnection::send(string &&message) {
    send(StringPiece(message));
}

void TcpConnection::send(const StringPiece &piece) {
    assert(status == Connected);
    if (loop->is_in_loop_thread()) {
        send_in_loop(piece);
    } else {
        void (TcpConnection::*fp)(const StringPiece &piece) = &TcpConnection::send_in_loop;
        loop->run_in_loop(bind(fp, this, piece));
    }
}

void TcpConnection::send_in_loop(const StringPiece &piece) {
    send_in_loop(piece.data(), piece.size());
}

void TcpConnection::send_in_loop(const void *data, size_t len) {
    assert(loop->is_in_loop_thread());
    outbound.append(data, len);
    conn_channel->enable_writing();
}

void TcpConnection::shutdown() {
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

void TcpConnection::set_context(void *ctx) {
    context = ctx;
}

void *TcpConnection::get_context() {
    return context;
}

Buffer &TcpConnection::inbound_buf() {
    return inbound;
}

Buffer &TcpConnection::outbound_buf() {
    return outbound;
}
