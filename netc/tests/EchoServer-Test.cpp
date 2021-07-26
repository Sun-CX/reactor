//
// Created by suncx on 2020/8/19.
//

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "ConsoleStream.h"

using std::placeholders::_1;
using std::placeholders::_2;
using reactor::net::EventLoop;
using reactor::net::TcpServer;
using reactor::net::TcpConnection;
using reactor::net::InetAddress;
using std::string;
using std::shared_ptr;
using reactor::core::Timestamp;

class EchoServer {
private:
    EventLoop *loop;
    TcpServer server;

    void on_data_arrived(const shared_ptr<TcpConnection> &conn, const Timestamp ts) {
        string msg = conn->in().retrieve_all_string();

        if (msg == "exit\n") {
            conn->out().append(msg);
            conn->close_safely();
            return;
        }

        if (msg == "quit\n") {
            conn->out().append("server is closing...\n");
            conn->send();
            conn->on_write_complete([this](const shared_ptr<TcpConnection> &con) {
                loop->quit();
            });
            return;
        }

        conn->out().append(msg);
        conn->send();
        conn->on_write_complete([](const shared_ptr<TcpConnection> &con) {
            RC_DEBUG << "has written completed!";
        });
    }

public:
    EchoServer(EventLoop *loop, const InetAddress listen_addr, int threads) :
            loop(loop),
            server(loop, listen_addr, "echo-svr", threads, true) {

        server.on_data(bind(&EchoServer::on_data_arrived, this, _1, _2));
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

