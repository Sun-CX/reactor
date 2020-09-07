//
// Created by suncx on 2020/8/19.
//

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "StringPiece.h"
#include "Timestamp.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class EchoServer {
private:
    EventLoop *loop;
    TcpServer server;

    void on_connection(const shared_ptr<TcpConnection> &conn) const {
        conn->send("hello\n");
    }

    void on_message(const shared_ptr<TcpConnection> &conn, Buffer *buf, Timestamp timestamp) {
        string msg(buf->retrieve_all_string());
        if (msg == "exit\n") {
            conn->send("bye.\n");
            conn->shutdown();
        }
        if (msg == "quit\n") {
            loop->quit();
        }
        conn->send(msg);
    }

public:
    EchoServer(EventLoop *loop, const InetAddress listen_addr) : loop(loop), server(loop, listen_addr, "echo-server") {
        server.set_conn_callback(bind(&EchoServer::on_connection, this, _1));
        server.set_msg_callback(bind(&EchoServer::on_message, this, _1, _2, _3));
    }

    void start() {
        server.start();
    }
};

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr(true, 8080);
    EchoServer server(&loop, addr);
    server.start();
    loop.loop();
    return 0;
}

