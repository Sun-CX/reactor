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

namespace reactor::net {
    using reactor::core::NonCopyable;
    using reactor::net::InetAddress;
    using reactor::net::Buffer;
    using std::enable_shared_from_this;
    using std::unique_ptr;
    using std::any;

    class EventLoop;

    class Channel;

    class Socket;

    class TcpConnection final : public NonCopyable, public enable_shared_from_this<TcpConnection> {
    private:
        // DISCONNECTING: tcp connection is on half-close state.
        enum STATUS {
            CONNECTING, CONNECTED, DISCONNECTING, DISCONNECTED
        };

        static const char *STATUS_STRING[4];

        EventLoop *const loop;
        STATUS status;
        bool reading;
        unique_ptr<Socket> socket;
        unique_ptr<Channel> conn_channel;
        const InetAddress local;
        const InetAddress peer;
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

        void shutdown();

//    void send_in_loop();

//    void send_in_loop(const StringPiece &piece);

//    void send_in_loop(const void *data, size_t len);

    public:
        TcpConnection(EventLoop *loop, int con_fd, const InetAddress &local, const InetAddress &peer);

        ~TcpConnection();

        void connection_established();

        void connection_destroyed();

        // send all of data in outbound buffer.
        // once data has been sent, channel's writable event will be disabled.
        void send();

        // ensure all data in outbound buffer has been sent and then close connection.
        void send_and_shutdown();

        void force_close();

        const any &get_context() const;

        void set_context(const any &ctx);

        Buffer &in();

        Buffer &out();

        int get_fd() const;

        const InetAddress &local_address() const;

        const InetAddress &peer_address() const;

        EventLoop *get_loop() const;

        void set_connection_callback(const ConnectionCallback &callback);

        void set_message_callback(const MessageCallback &callback);

        void set_write_complete_callback(const WriteCompleteCallback &callback);

        void set_close_callback(const CloseCallback &callback);
    };
}

#endif //REACTOR_TCPCONNECTION_H