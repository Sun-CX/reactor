//
// Created by suncx on 2020/9/23.
//

#include <HttpRequest.h>
#include "HttpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"

using std::find;

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr(true, 8080);
    HttpServer httpServer(&loop, addr, "http-server", 3, true);
    httpServer.start();
    loop.loop();
    return 0;
}
