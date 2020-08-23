//
// Created by suncx on 2020/8/23.
//

#include "EventLoop.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include <unistd.h>

void on_new_connection(int con_fd, const InetAddress &cli) {
    printf("con_fd: %d, client: %s\n", con_fd, to_readable_string(cli).c_str());

    char buf[2014];
    auto n = read(con_fd, buf, sizeof(buf));
    printf("received: %.*s\n", (int) n, buf);
//    close(con_fd);
}

int main(int argc, const char *argv[]) {

    EventLoop loop;

    InetAddress addr(true, 8080);
    Acceptor acceptor(&loop, addr, true);
    acceptor.set_connection_callback(on_new_connection);

    acceptor.listen();
    loop.loop();
    return 0;
}
