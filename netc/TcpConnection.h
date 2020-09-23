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

class StringPiece;

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
    unique_ptr<Channel> conn_channel;
    const InetAddress local;
    const InetAddress peer;
    size_t high_water_mark;
    Buffer inbound;     // 入站缓冲区
    Buffer outbound;    // 出站缓冲区
    void *context;

    ConnectionCallback conn_callback;
    MessageCallback msg_callback;
    CloseCallback close_callback;
    WriteCompleteCallback write_complete_callback;
    HighWaterMarkCallback high_water_mark_callback;

    void read_handler();

    void write_handler();

    void close_handler();

    void error_handler();

    void shutdown_in_loop();

    void send_in_loop(const StringPiece &piece);

    void send_in_loop(const void *data, size_t len);

public:
    TcpConnection(EventLoop *loop, string name, int con_fd, const InetAddress &local, const InetAddress &peer);

    virtual ~TcpConnection();

    void set_connection_callback(const ConnectionCallback &callback);

    void set_message_callback(const MessageCallback &callback);

    void set_write_complete_callback(const WriteCompleteCallback &callback);

    void set_high_water_mark_callback(const HighWaterMarkCallback &callback);

    void set_close_callback(const CloseCallback &callback);

    void connection_established();

    void connection_destroyed();

    bool connected() const;

    void send(const StringPiece &piece);

    void send(string &&message);

    void shutdown();

    void set_context(void *ctx);

    void *get_context();

    Buffer &inbound_buf();

    Buffer &outbound_buf();

    const string &get_name() const;

    const InetAddress &local_address() const;

    const InetAddress &peer_address() const;

    EventLoop *get_loop() const;
};

#endif //REACTOR_TCPCONNECTION_H
