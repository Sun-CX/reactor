//
// Created by suncx on 2020/9/23.
//

#include "HttpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"

void service(const HttpRequest &request, HttpResponse &response) {
    response.set_response_status(200, "OK");
    response.set_header("Content-Type","text/plain");
    response.set_body("hello reactor");
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
