//
// Created by suncx on 2020/8/19.
//

#ifndef REACTOR_TCPCONNECTION_H
#define REACTOR_TCPCONNECTION_H

#include "NonCopyable.h"
#include "InetAddress.h"
#include "Events.h"
#include "Buffer.h"

using std::unique_ptr;
using std::enable_shared_from_this;

class EventLoop;

class Channel;

class Socket;

class TcpConnection final : public NonCopyable, public enable_shared_from_this<TcpConnection> {
private:
    enum STATUS {
        Connecting, Connected, Disconnecting, Disconnected
    };

    EventLoop *loop;
    const string name;
    STATUS status;
    bool reading;
    unique_ptr<Socket> socket;
    unique_ptr<Channel> channel;
    const InetAddress local;
    const InetAddress peer;

    ConnectionCallback conn_callback;
    MessageCallback msg_callback;
    WriteCompleteCallback write_complete_callback;
    HighWaterMarkCallback high_water_mark_callback;
    CloseCallback close_callback;

    size_t high_water_mark;
    Buffer input_buffer;
    Buffer output_buffer;

    void read_handler();

    void write_handler();

    void close_handler();

    void error_handler();

public:
    TcpConnection(EventLoop *loop, string name, int sock_fd, const InetAddress &local, const InetAddress &peer);

    void set_connection_callback(const ConnectionCallback &callback);

    void set_message_callback(const MessageCallback &callback);

    void set_write_complete_callback(const WriteCompleteCallback &callback);

    void set_high_water_mark_callback(const HighWaterMarkCallback &callback);

    void set_close_callback(const CloseCallback &callback);

    void connection_established();
};

#endif //REACTOR_TCPCONNECTION_H
