//
// Created by suncx on 2020/8/19.
//

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Timestamp.h"
#include "ConsoleStream.h"

using std::placeholders::_1;
using std::placeholders::_2;
using reactor::core::Timestamp;
using reactor::net::EventLoop;
using reactor::net::TcpServer;
using reactor::net::TcpConnection;
using reactor::net::InetAddress;
using std::string;
using std::shared_ptr;

class EchoServer {
private:
    EventLoop *loop;
    TcpServer server;

    void on_message(const shared_ptr<TcpConnection> &conn, Timestamp timestamp) {
        RC_DEBUG << "on_message called!";
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
        conn->set_write_complete_callback([](const shared_ptr<TcpConnection> &con) {
            RC_DEBUG << "has written completed!";
        });
    }

public:
    EchoServer(EventLoop *loop, const InetAddress listen_addr, int threads) :
            loop(loop),
            server(loop, listen_addr, "echo-svr", threads, true) {

        server.set_message_callback(bind(&EchoServer::on_message, this, _1, _2));
    }

    void start() { server.start(); }
};

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr("192.168.2.2", 8080);
    EchoServer server(&loop, addr, 3);
    server.start();
    loop.loop();
    return 0;
}

