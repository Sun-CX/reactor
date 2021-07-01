//
// Created by suncx on 2020/9/23.
//

#include "HttpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "ConsoleStream.h"

using std::string;
using std::to_string;
using reactor::net::HttpRequest;
using reactor::net::HttpResponse;
using reactor::net::http::Headers;
using reactor::net::EventLoop;
using reactor::net::InetAddress;
using reactor::net::HttpServer;

void service(const HttpRequest &request, HttpResponse &response) {

    const Headers &headers = request.get_headers();
    for (const auto &e: headers) {
        DEBUG << e.first << ':' << e.second;
    }


    response.set_response_status(200, "OK");
    response.set_header("Content-Type", "text/plain");
    string body("hello reactor!");
    response.set_body(body);
    response.set_header("Content-Length", to_string(body.size()));
}

int main(int argc, const char *argv[]) {
    EventLoop loop;
    InetAddress addr("192.168.2.2", 8080);
    HttpServer httpServer(&loop, addr, "http-svr", 3, true);
    httpServer.set_http_callback(service);
    httpServer.start();
    loop.loop();
    return 0;
}
