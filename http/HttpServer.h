//
// Created by suncx on 2020/9/23.
//

#ifndef REACTOR_HTTPSERVER_H
#define REACTOR_HTTPSERVER_H

#include "TcpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpServer final : public NonCopyable {
private:
    using HttpCallback = function<void(const HttpRequest &, HttpResponse &)>;
    TcpServer server;
    HttpCallback http_callback;

    void on_connection(const shared_ptr<TcpConnection> &connection) const;

    void on_message(const shared_ptr<TcpConnection> &connection, Timestamp recv_time);

public:
    HttpServer(EventLoop *loop, const InetAddress &addr, string name, int threads, bool reuse_port = false);

    void set_http_callback(const HttpCallback &callback);

    void start();
};

#endif //REACTOR_HTTPSERVER_H