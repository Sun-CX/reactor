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
    InetAddress addr(true, 8080);
    HttpServer httpServer(&loop, addr, "http-svr", 3, true);
    httpServer.set_http_callback(service);
    httpServer.start();
    loop.loop();
    return 0;
}
```