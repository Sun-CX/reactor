//
// Created by suncx on 2020/8/19.
//

#ifndef REACTOR_TCPCONNECTION_H
#define REACTOR_TCPCONNECTION_H

#include "NonCopyable.h"
#include "InetAddress.h"
#include "Events.h"
#include "Buffer.h"
#include <any>

using std::unique_ptr;
using std::enable_shared_from_this;
using std::any;

class EventLoop;

class Channel;

class Socket;

class TcpConnection final : public NonCopyable, public enable_shared_from_this<TcpConnection> {
public:
    enum STATUS {
        Connecting, Connected, Disconnecting, Disconnected
    };
private:
    static const char *STATUS_STR[4];

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
    any context;

    ConnectionCallback conn_callback;
    MessageCallback msg_callback;
    CloseCallback close_callback;
    WriteCompleteCallback write_complete_callback;

    void read_handler();

    void write_handler();

    void close_handler();

    void error_handler();

    void shutdown_in_loop();

//    void send_in_loop();

//    void send_in_loop(const StringPiece &piece);

//    void send_in_loop(const void *data, size_t len);

public:
    TcpConnection(EventLoop *loop, string name, int con_fd, const InetAddress &local, const InetAddress &peer);

    virtual ~TcpConnection();

    void set_connection_callback(const ConnectionCallback &callback);

    void set_message_callback(const MessageCallback &callback);

    void set_write_complete_callback(const WriteCompleteCallback &callback);

    void set_close_callback(const CloseCallback &callback);

    void connection_established();

    void connection_destroyed();

    bool connected() const;

    void set_status(STATUS status);

    STATUS get_status() const;

//    void send(const StringPiece &piece);

//    void send(const void *begin, size_t n);
    void send_outbound_bytes();

    void shutdown();

    const any &get_context() const;

    void set_context(const any &ctx);

    Buffer &inbound_buf();

    Buffer &outbound_buf();

    const string &get_name() const;

    const InetAddress &local_address() const;

    const InetAddress &peer_address() const;

    EventLoop *get_loop() const;
};

#endif //REACTOR_TCPCONNECTION_H
