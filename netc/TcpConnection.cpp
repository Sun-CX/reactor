//
// Created by suncx on 2020/8/19.
//

#include "TcpConnection.h"
#include "Exception.h"
#include "EventLoop.h"
#include "Timestamp.h"
#include "Socket.h"
#include "Channel.h"
#include <unistd.h>
#include <cassert>

using std::bind;

TcpConnection::TcpConnection(EventLoop *loop, string name, int sock_fd, const InetAddress &local,
                             const InetAddress &peer) : loop(loop), name(move(name)),
                                                        status(Connecting), reading(true), socket(new Socket(sock_fd)),
                                                        channel(new Channel(loop, sock_fd)),
                                                        local(local), peer(peer), high_water_mark(64 * 1024 * 1024) {
    channel->set_read_callback(bind(&TcpConnection::read_handler, this));
    channel->set_write_callback(bind(&TcpConnection::write_handler, this));
    channel->set_close_callback(bind(&TcpConnection::close_handler, this));
    channel->set_error_callback(bind(&TcpConnection::error_handler, this));
    socket->keep_alive(true);
}

void TcpConnection::read_handler() {
    assert(loop->is_in_loop_thread());
    int saved_errno = 0;
    ssize_t n = input_buffer.read_from_fd(channel->get_fd(), &saved_errno);
    if (n > 0) {
        //TODO:fix timestamp.
        msg_callback(shared_from_this(), &input_buffer, Timestamp());
    } else if (n == 0) {
        close_handler();
    } else {
        errno = saved_errno;
        fprintf(stderr, "read_handler from TcpConnection error.");
        error_handler();
    }
}

void TcpConnection::write_handler() {
    assert(loop->is_in_loop_thread());
    if (channel->is_writing()) {
        auto n = write(channel->get_fd(), output_buffer.peek(), output_buffer.readable_bytes());
        if (n > 0) {
            output_buffer.retrieve(n);
            if (output_buffer.readable_bytes() == 0) {
                channel->disable_writing();
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
        printf("Connection fd: %d is down, no more writing...\n", channel->get_fd());
    }
}

void TcpConnection::close_handler() {
    assert(loop->is_in_loop_thread());
    assert(status == Connected or status == Disconnecting);
    status = Disconnected;
    channel->disable_all();

    shared_ptr<TcpConnection> guard_this(shared_from_this());
    //TODO: why?
    conn_callback(guard_this);
    close_callback(guard_this);
}

void TcpConnection::error_handler() {
    int opt;
    socklen_t len = sizeof(opt);
    auto n = getsockopt(socket->fd(), SOL_SOCKET, SO_ERROR, &opt, &len);
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

    channel->enable_reading();
    conn_callback(shared_from_this());
}

void TcpConnection::shutdown_in_loop() {
    assert(loop->is_in_loop_thread());
    if (!channel->is_writing()) {
//        socket->shutdown_write();
        auto n = shutdown(socket->fd(), SHUT_WR);
        if (unlikely(n < 0)) ERROR_EXIT("shutdown error.");
    }
}

TcpConnection::~TcpConnection() {
    assert(status == Disconnected);
}

