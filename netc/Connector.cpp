//
// Created by suncx on 2020/8/23.
//

#include "Connector.h"
#include "ConsoleStream.h"
#include "EventLoop.h"
#include "Channel.h"
#include <cassert>
#include <memory>
#include <unistd.h>

using std::make_unique;
using reactor::net::Connector;
using reactor::net::InetAddress;
using reactor::net::EventLoop;
using reactor::core::operator ""_s;

Connector::Connector(EventLoop *loop, const InetAddress &addr) :
        loop(loop),
        server_addr(addr),
        status(DISCONNECTED),
        channel(),
        connection_callback(),
        retry_delay_ms(30 * 1000) {}

Connector::~Connector() {
    RC_DEBUG << "-------------- -Connector --------------";
}

void Connector::set_new_conn_callback(const NewConnectionCallback &callback) {
    connection_callback = callback;
}

const InetAddress &Connector::get_server_addr() const {
    return server_addr;
}

void Connector::start() {
    loop->run_in_loop(bind(&Connector::start_in_loop, this));
}

void Connector::start_in_loop() {
    assert(loop->is_in_loop_thread());
    assert(status == DISCONNECTED);
    connect();
}

void Connector::connect() {
    int sock_fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sock_fd < 0) RC_FATAL << "socket create error";

    int rc = ::connect(sock_fd, server_addr.get_sockaddr(), sizeof(server_addr));
    int err = rc == 0 ? 0 : errno;

    switch (err) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sock_fd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sock_fd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            RC_ERROR << "Connect error: " << err;
            ::close(sock_fd);
            break;

        default:
            RC_FATAL << "Unexpected error: " << err;
            ::close(sock_fd);
            break;
    }
}

void Connector::handle_write() {
    assert(status == CONNECTING);

    int sock_fd = remove_and_reset_channel();

    int err = get_sock_err(sock_fd);

    if (err != 0) {
        RC_ERROR << "Connector SO_ERROR: " << err;
        retry(sock_fd);
    } else if (is_self_connect(sock_fd)) {
        RC_ERROR << "Connector self connecting";
        retry(sock_fd);
    } else {
        status = CONNECTED;
        connection_callback(sock_fd);
    }
}

int Connector::remove_and_reset_channel() {
    channel->disable_all();
    channel->remove();

    int sock_fd = channel->get_fd();
    loop->queue_in_loop(bind(&Connector::reset_channel, this));
    return sock_fd;
}

void Connector::handle_error() {
    assert(status == CONNECTING);
    int sock = remove_and_reset_channel();
    int err = get_sock_err(sock);
    RC_ERROR << "Connector SOL_ERROR: " << err;
    retry(sock);
}

void Connector::stop() {
    loop->queue_in_loop(bind(&Connector::stop_in_loop, this));
}

void Connector::stop_in_loop() {
    assert(loop->is_in_loop_thread());

    if (status == CONNECTING) {
        status = DISCONNECTED;
        int fd = remove_and_reset_channel();
        retry(fd);
    }
}

void Connector::connecting(int sock_fd) {
    assert(status == DISCONNECTED);

    status = CONNECTING;
    assert(channel == nullptr);

    channel = make_unique<Channel>(loop, sock_fd);
    channel->set_write_callback(bind(&Connector::handle_write, this));
    channel->set_error_callback(bind(&Connector::handle_error, this));

    channel->enable_writing();
}

void Connector::reset_channel() {
    assert(channel != nullptr);
    channel.reset();
}

bool Connector::is_self_connect(int fd) const {
    //TODO: self connect!
    return false;
}

int Connector::get_sock_err(int sock_fd) const {
    int opt;
    socklen_t socklen = sizeof(opt);
    if (::getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &opt, &socklen) < 0) {
        return errno;
    } else return opt;
}

void Connector::retry(int sock_fd) {
    int rc = ::close(sock_fd);
    if (rc < 0) RC_ERROR << "Socket " << sock_fd << " close error";

    status = DISCONNECTED;

    RC_TRACE << "Connector retry...";

    loop->schedule(bind(&Connector::start_in_loop, shared_from_this()), 0_s, 2_s);
}