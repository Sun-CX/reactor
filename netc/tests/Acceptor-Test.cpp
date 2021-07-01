//
// Created by suncx on 2020/8/23.
//

#include "EventLoop.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include <unistd.h>
#include "ConsoleStream.h"

using reactor::net::EventLoop;
using reactor::net::InetAddress;
using reactor::net::Acceptor;

static void on_new_connection(int con_fd, const InetAddress &cli) {
    DEBUG << "con_fd: " << con_fd << ", client: " << cli.to_string();
    ::close(con_fd);
}

int main(int argc, const char *argv[]) {

    EventLoop loop;

    InetAddress addr("192.168.2.2", 8080);
    Acceptor acceptor(&loop, addr, true);
    acceptor.set_new_connection_callback(on_new_connection);
    acceptor.listen();

    loop.loop();
    return 0;
}
