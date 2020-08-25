//
// Created by suncx on 2020/8/19.
//

#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <unistd.h>
#include <cassert>

using std::bind;
using std::placeholders::_1;

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
//    loop->assert_in_created_thread();
    assert(loop->is_in_loop_thread());
    int saved_errno = 0;
    ssize_t n = input_buffer.read_from_fd(channel->get_fd(), &saved_errno);
    if (n > 0) {
//        msg_callback(shared_from_this(), &input_buffer, recv_time);
    } else if (n == 0) {
        close_handler();
    } else {
        errno = saved_errno;
        fprintf(stderr, "read_handler from TcpConnection error.");
        error_handler();
    }
}

void TcpConnection::write_handler() {
//    loop->assert_in_created_thread();
    assert(loop->is_in_loop_thread());
    if (channel->is_writing()) {
        auto n = write(channel->get_fd(), output_buffer.peek(), output_buffer.readable_bytes());
        if (n > 0) {
            output_buffer.retrieve(n);
            if (output_buffer.readable_bytes() == 0) {
                channel->disable_writing();
                if (write_complete_callback) {

                }
                if (status == Disconnecting) {

                }
            }
        } else {
            fprintf(stderr, "write error.\n");
        }
    } else {

    }
}

void TcpConnection::close_handler() {

}

void TcpConnection::error_handler() {

}

void TcpConnection::set_connection_callback(const ConnectionCallback &callback) {

}

void TcpConnection::set_message_callback(const MessageCallback &callback) {

}

void TcpConnection::set_write_complete_callback(const WriteCompleteCallback &callback) {

}

void TcpConnection::set_high_water_mark_callback(const HighWaterMarkCallback &callback) {

}

void TcpConnection::set_close_callback(const CloseCallback &callback) {

}

void TcpConnection::connection_established() {

}

