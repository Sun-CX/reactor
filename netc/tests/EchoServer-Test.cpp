//
// Created by suncx on 2020/8/19.
//

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Timestamp.h"

using std::placeholders::_1;
using std::placeholders::_2;

class EchoServer {
private:
    EventLoop *loop;
    TcpServer server;

    void on_message(const shared_ptr<TcpConnection> &conn, Timestamp timestamp) {
        string msg = conn->inbound_buf().retrieve_all_string();
        if (msg == "exit\n") {
            conn->outbound_buf().append("bye.\n");
            conn->send_outbound_bytes();
            conn->set_write_complete_callback([](const shared_ptr<TcpConnection> &con) {
                con->shutdown();
            });
            return;
        }
        if (msg == "quit\n") {
            conn->outbound_buf().append("server is closing...");
            conn->send_outbound_bytes();
            conn->set_write_complete_callback([this](const shared_ptr<TcpConnection> &con) {
                loop->quit();
            });
            return;
        }
        conn->outbound_buf().append(msg);
        conn->send_outbound_bytes();
    }

public:
    EchoServer(EventLoop *loop, const InetAddress listen_addr, int threads) : loop(loop), server(loop, listen_addr, "echo-svr", threads, true) {
        server.set_msg_callback(bind(&EchoServer::on_message, this, _1, _2));
    }

    void start() { server.start(); }
};

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr("192.168.2.2", 8080);
    EchoServer server(&loop, addr, 8);
    server.start();
    loop.loop();
    return 0;
}

