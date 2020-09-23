//
// Created by suncx on 2020/9/23.
//

#include "HttpServer.h"
#include "Timestamp.h"
#include "TcpConnection.h"
#include "HttpContext.h"
#include <cassert>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void default_http_callback(const HttpRequest &request, HttpResponse *response) {
    printf("TODO...\n");
}

HttpServer::HttpServer(EventLoop *loop, const InetAddress &addr, string name, int threads, bool reuse_port)
        : server(loop, addr, move(name), threads, reuse_port),
          http_callback(default_http_callback) {
    server.set_conn_callback(bind(&HttpServer::on_connection, this, _1));
    server.set_msg_callback(bind(&HttpServer::on_message, this, _1, _2, _3));
}

void HttpServer::on_connection(const shared_ptr<TcpConnection> &connection) const {
    assert(connection->connected());
    printf("Http client %s connected...\n", connection->peer_address().to_string().c_str());
    connection->set_context(new HttpContext());
}

void HttpServer::on_message(const shared_ptr<TcpConnection> &connection, Buffer *buf, Timestamp recv_time) {
    auto context = static_cast<HttpContext *>(connection->get_context());
    auto r = context->parse_request(buf);
    if (r) {
//        connection->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        connection->shutdown();
    }

    if (context->parse_success()) {
        on_request(connection, context->get_request());
        context->reset();
    }
}

void HttpServer::on_request(const shared_ptr<TcpConnection> &connection, const HttpRequest &request) {
    auto s = request.get_header("Host");
    printf("hahhahaah=============%s\n", s.c_str());
}

void HttpServer::start() {
    server.start();
}

void HttpServer::set_http_callback(const HttpServer::HttpCallback &callback) {
    http_callback = callback;
}
