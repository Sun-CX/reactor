//
// Created by suncx on 2020/9/23.
//

#include "HttpServer.h"
#include "Timestamp.h"
#include "TcpConnection.h"
#include "HttpContext.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::any_cast;

static void default_http_callback(const HttpRequest &request, HttpResponse &response) {
    printf("TODO...\n");
}

HttpServer::HttpServer(EventLoop *loop, const InetAddress &addr, string name, int threads, bool reuse_port) :
        server(loop, addr, move(name), threads, reuse_port), http_callback(default_http_callback) {
    server.set_conn_callback(bind(&HttpServer::on_connection, this, _1));
    server.set_msg_callback(bind(&HttpServer::on_message, this, _1, _2));
}

void HttpServer::on_connection(const shared_ptr<TcpConnection> &connection) const {
    connection->set_context(HttpContext());
}

void HttpServer::on_message(const shared_ptr<TcpConnection> &connection, Timestamp recv_time) {
    auto context = any_cast<HttpContext>(connection->get_context());
    auto parse_success = context.parse_request(connection->inbound_buf());

    auto version = context.get_request().get_version();
    context.get_response().set_version(version);
    context.get_response().set_header("Server", "reactor");
    context.get_response().set_header("Date", Timestamp::now().to_string());

    if (!parse_success) {
        context.get_response().set_response_status(400, "Bad Request");
    } else {
        http_callback(context.get_request(), context.get_response());
    }
    context.parse_response(connection->outbound_buf());
    connection->send_outbound_bytes();
}

void HttpServer::start() {
    server.start();
}

void HttpServer::set_http_callback(const HttpServer::HttpCallback &callback) {
    http_callback = callback;
}
