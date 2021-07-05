//
// Created by 孙诚雄 on 2021/7/5.
//

#include "TcpServer.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include "MessageCodec.h"
#include "query.pb.h"

using reactor::net::TcpServer;
using reactor::net::TcpConnection;
using reactor::proto::Dispatcher;
using reactor::proto::MessageCodec;
using reactor::net::EventLoop;
using reactor::net::InetAddress;
using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using google::protobuf::Message;
using reactor::core::Timestamp;
using reactor::core::NonCopyable;
using reactor::proto::examples::Query;
using reactor::proto::examples::Answer;

class QueryServer final : public NonCopyable {
private:
    TcpServer server;
    Dispatcher dispatcher;
    MessageCodec codec;

    void on_connection(const shared_ptr<TcpConnection> &con) const {
        RC_DEBUG << "New Connection: " << con->peer_address().to_string()
                 << " ---> " << con->local_address().to_string();
    }

    void on_unknown_msg_type(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) const {
        RC_WARN << "Unknown message: " << msg->GetTypeName();
    }

    void on_query(const shared_ptr<TcpConnection> &con, const shared_ptr<Query> &query, Timestamp ts) {
        RC_DEBUG << "On Query(" << query->GetTypeName() << "): " << query->DebugString();

        Answer answer;
        answer.set_id(1);
        answer.set_questioner("sun-cx");
        answer.set_answer("https://github.com/Sun-CX");
        answer.set_solution("OK");
        codec.send_message(con, answer);

        con->shutdown();
    }

    void on_answer(const shared_ptr<TcpConnection> &con, const shared_ptr<Answer> &answer, Timestamp ts) const {
        RC_DEBUG << "On Answer: " << answer->GetTypeName();
        con->shutdown();
    }

public:
    QueryServer(EventLoop *loop, const InetAddress &addr) :
            server(loop, addr, "query-server", 4, true),
            dispatcher(bind(&QueryServer::on_unknown_msg_type, this, _1, _2, _3)),
            codec(bind(&Dispatcher::on_proto_message, &dispatcher, _1, _2, _3)) {

        dispatcher.register_message_callback<Query>(bind(&QueryServer::on_query, this, _1, _2, _3));
        dispatcher.register_message_callback<Answer>(bind(&QueryServer::on_answer, this, _1, _2, _3));

        server.set_new_connection_callback(bind(&QueryServer::on_connection, this, _1));
        server.set_message_callback(bind(&MessageCodec::on_message, &codec, _1, _2));
    }

    void start() {
        server.start();
    }
};

int main(int argc, const char *argv[]) {
    EventLoop loop;

    InetAddress bind_addr("192.168.2.2", 8080);
    QueryServer server(&loop, bind_addr);
    server.start();

    loop.loop();

    return 0;
}