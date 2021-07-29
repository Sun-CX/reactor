//
// Created by 孙诚雄 on 2021/7/26.
//

#include "EventLoop.h"
#include "TcpClient.h"
#include "TcpConnection.h"
#include "ConsoleStream.h"

using reactor::core::NonCopyable;
using reactor::core::Timestamp;
using reactor::net::TcpClient;
using reactor::net::EventLoop;
using reactor::net::InetAddress;
using reactor::net::TcpConnection;
using std::string;
using std::shared_ptr;
using std::placeholders::_1;
using std::placeholders::_2;

class EchoClient final : public NonCopyable {
private:
    EventLoop *loop;
    TcpClient client;

    void on_connect(const shared_ptr<TcpConnection> &con) const {
        con->out().append("hello world");
        con->send();
    }

    void on_message(const shared_ptr<TcpConnection> &con, Timestamp ts) const {
        string s = con->in().retrieve_all_string();
        RC_DEBUG << s;
        con->close();
        // loop->quit();
    }

public:
    EchoClient(EventLoop *loop, const InetAddress &addr) :
            loop(loop),
            client(loop, addr, "cli") {

        client.on_connect(bind(&EchoClient::on_connect, this, _1));
        client.on_data(bind(&EchoClient::on_message, this, _1, _2));
    }

    void connect() {
        client.connect();
    }
};

int main(int argc, const char *argv[]) {

    EventLoop loop;

    InetAddress addr("192.168.2.2", 8080);
    EchoClient client(&loop, addr);

    client.connect();

    loop.loop();

    return 0;
}