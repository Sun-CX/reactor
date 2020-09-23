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

class EchoServer {
private:
    EventLoop *loop;
    TcpServer server;

    void on_message(const shared_ptr<TcpConnection> &conn, Timestamp timestamp) {
        string msg = conn->inbound_buf().retrieve_all_string();
        if (msg == "exit\n") {
            conn->send(StringPiece("bye.\n"));
            conn->shutdown();
        }
        if (msg == "quit\n") {
            loop->quit();
        }
        conn->send(msg);
    }

public:
    EchoServer(EventLoop *loop, const InetAddress listen_addr) : loop(loop), server(loop, listen_addr, "loop", 4,
                                                                                    false) {
        server.set_msg_callback(bind(&EchoServer::on_message, this, _1, _2));
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

