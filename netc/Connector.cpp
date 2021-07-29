//
// Created by suncx on 2020/8/23.
//

#include "Connector.h"
#include "ConsoleStream.h"
#include "Channel.h"
#include "Ext.h"
#include <cassert>
#include <cstring>
#include <unistd.h>

using std::make_unique;
using std::chrono_literals::operator ""s;
using reactor::net::Connector;
using reactor::net::InetAddress;
using reactor::net::EventLoop;

Connector::Connector(EventLoop *loop, const InetAddress &addr) :
        loop(loop),
        server_addr(addr),
        status(DISCONNECTED),
        channel(),
        con_handler(),
        cur_task(),
        enable_retry(true) {}

Connector::~Connector() {
    assert(channel == nullptr);
    RC_DEBUG << "-------------- -Connector --------------";
}

void Connector::on_connect(const NewConnectionHandler &handler) {
    con_handler = handler;
}

void Connector::start() {
    loop->run_in_loop(bind(&Connector::start_in_loop, this));
}

void Connector::start_in_loop() {
    assert(loop->is_in_created_thread());
    assert(status == DISCONNECTED);
    connect();
}

void Connector::connect() {
    int fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (fd < 0) RC_FATAL << "socket create error: " << ::strerror(errno);

    int ret = ::connect(fd, server_addr.get_sockaddr(), sizeof(server_addr));
    int err = ret == 0 ? 0 : errno;

    switch (err) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(fd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(fd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            close(fd);
            break;

        default:
            close(fd);
            break;
    }
}

void Connector::connecting(int fd) {
    assert(status == DISCONNECTED);

    status = CONNECTING;
    assert(channel == nullptr);

    channel = make_unique<Channel>(loop, fd);
    channel->on_write(bind(&Connector::handle_write, this));
    channel->on_close(bind(&Connector::handle_close, this));
    channel->on_error(bind(&Connector::handle_error, this));
    channel->enable_writing();
}

void Connector::handle_write() {
    assert(loop->is_in_created_thread());
    assert(status == CONNECTING);

    int fd = channel->get_fd();
    int err = get_sock_err(fd);

    if (err == 0) {

        remove_and_reset_channel();

        status = CONNECTED;
        con_handler(fd);

    } else
        RC_ERROR << "socket(" << fd << ") error: " << ::strerror(err);
}

void Connector::handle_close() {
    assert(loop->is_in_created_thread());
    assert(status == CONNECTING);

    remove_and_reset_channel();

    loop->queue_in_loop(bind(&Connector::retry, this, channel->get_fd()));
}

void Connector::handle_error() {
    assert(loop->is_in_created_thread());
    assert(status == CONNECTING);
}

void Connector::remove_and_reset_channel() {
    channel->remove();
    loop->queue_in_loop(bind(&Connector::reset_channel, this));
}

void Connector::reset_channel() {
    assert(channel != nullptr);
    channel.reset();
    assert(channel == nullptr);
}

void Connector::stop() {
    loop->queue_in_loop(bind(&Connector::stop_in_loop, this));
}

void Connector::stop_in_loop() {
    assert(loop->is_in_created_thread());

    if (status == DISCONNECTED) {
        if (cur_task) {
            loop->cancel(cur_task);
            cur_task.reset();

            assert(cur_task == nullptr);
        }
    } else if (status == CONNECTING) {
        enable_retry = false;
    }
}

bool Connector::is_self_connect(int fd) const {
    //TODO: self connect!
    return false;
}

int Connector::get_sock_err(const int sock) const {
    int opt;
    socklen_t len = sizeof(opt);
    if (unlikely(::getsockopt(sock, SOL_SOCKET, SO_ERROR, &opt, &len) < 0)) {
        RC_FATAL << "getsockopt(" << sock << ") error: " << ::strerror(errno);
        return errno;
    } else return opt;
}

void Connector::retry(int fd) {
    assert(status == CONNECTING);

    close(fd);

    status = DISCONNECTED;

    cur_task.reset();

    if (enable_retry) {
        assert(cur_task == nullptr);

        RC_DEBUG << "retry...";
        cur_task = loop->schedule(bind(&Connector::start_in_loop, this), 2s);
    }
}

void Connector::close(int fd) const {
    if (unlikely(::close(fd) < 0))
        RC_FATAL << "socket(" << fd << ") close error: " << ::strerror(errno);
}