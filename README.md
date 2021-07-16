# reactor
reactor is a network communicating library written in C++ 17.

## Features
enlightened by muduo from chenshuo, reactor has all advantages muduo has and also has
more improvements:
1. purely written in C++ 17, more modernized code style, doesn't require any third part dependencies(boost).
2. fix some vulnerabilities in muduo.
3. redesign Socket, distinguish [ServerSocket](https://github.com/Sun-CX/reactor/blob/master/netc/ServerSocket.h) for listening and [Socket](https://github.com/Sun-CX/reactor/blob/master/netc/Socket.h) for communicating.
4. implement [CircularBuffer](https://github.com/Sun-CX/reactor/blob/master/core/CircularBuffer.h) instead of `boost::circular_buffer`.
5. redesign timer by [QuadHeap](https://github.com/Sun-CX/reactor/blob/master/netc/MinHeap.h) instead of `std::set`.

## Startup

### A TCP echo-server

```cpp
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
        string msg = conn->in().retrieve_all_string();
        if (msg == "exit\n") {
            conn->out().append(msg);
            conn->send_and_shutdown();
            return;
        }
        if (msg == "quit\n") {
            conn->out().append("server is closing...\n");
            conn->send();
            conn->set_write_complete_callback([this](const shared_ptr<TcpConnection> &con) {
                loop->quit();
            });
            return;
        }
        conn->out().append(msg);
        conn->send();
    }

public:
    EchoServer(EventLoop *loop, const InetAddress listen_addr, int threads) : loop(loop), server(loop, listen_addr, "echo-svr", threads, true) {
        server.set_msg_callback(bind(&EchoServer::on_message, this, _1, _2));
    }

    void start() {
        server.start();
    }
};

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8080);
    EchoServer server(&loop, addr, 10);
    server.start();
    loop.loop();
    return 0;
}
```

### A simple HTTP service

```cpp
#include "HttpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"

void service(const HttpRequest &request, HttpResponse &response) {
    response.set_response_status(200, "OK");
    response.set_header("Content-Type","text/plain");
    response.set_body("hello reactor!");
}

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8080);
    HttpServer httpServer(&loop, addr, "http-svr", 3, true);
    httpServer.set_http_callback(service);
    httpServer.start();
    loop.loop();
    return 0;
}
```