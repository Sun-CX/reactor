//
// Created by 孙诚雄 on 2021/7/26.
//

#include "Thread.h"
#include "Connector.h"
#include "ConsoleStream.h"

using reactor::core::Thread;
using reactor::net::EventLoop;
using reactor::net::Connector;
using reactor::net::InetAddress;
using std::chrono_literals::operator ""s;

static void new_connection(int fd) {
    RC_DEBUG << "Connected...";
}

int main(int argc, const char *argv[]) {

    EventLoop loop;

    InetAddress address("192.168.2.2", 8080);
    Connector connector(&loop, address);
    connector.on_connection(new_connection);

    Thread t([&connector] {
        EventLoop loop1;

        loop1.schedule([&connector, &loop1] {
            connector.stop();
            loop1.quit();
        }, 3s);

        loop1.loop();
    });

    t.start();
    connector.start();

    loop.loop();
    t.join();

    return 0;
}