//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_ACCEPTOR_H
#define REACTOR_ACCEPTOR_H

#include "Socket.h"
#include "Channel.h"

class EventLoop;

class Acceptor final : public NonCopyable {
private:
    using ConnectionCallback = function<void(int, const InetAddress &)>;

    EventLoop *loop;
    Socket accept_socket;
    Channel accept_channel;
    ConnectionCallback callback;
    bool listening;
    int idle_fd;

    int create_socket() const;

    void read_handler();

public:
    Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port);

    virtual ~Acceptor();

    void set_read_handler(const ConnectionCallback &handler);

    void listen();

    bool is_listening() const;
};


#endif //REACTOR_ACCEPTOR_H
