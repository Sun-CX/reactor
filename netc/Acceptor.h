//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_ACCEPTOR_H
#define REACTOR_ACCEPTOR_H

#include "ServerSocket.h"
#include "Channel.h"

class EventLoop;

// 在上层应用程序中，一般不直接使用 Acceptor，而是把它作为 TcpServer 的成员
class Acceptor final : public NonCopyable {
private:
    using NewConnectionCallback = function<void(int, const InetAddress &)>;

    EventLoop *loop;
    ServerSocket server_socket;
    Channel accept_channel;
    NewConnectionCallback callback;
    bool listening;
    int idle_fd;

    void read_handler();

public:
    Acceptor(EventLoop *loop, const InetAddress &addr, bool reuse_port);

    virtual ~Acceptor();

    void set_new_connection_callback(const NewConnectionCallback &handler);

    void listen();

    [[nodiscard]]
    bool is_listening() const;
};

#endif //REACTOR_ACCEPTOR_H