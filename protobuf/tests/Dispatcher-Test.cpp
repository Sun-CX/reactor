//
// Created by 孙诚雄 on 2021/7/5.
//

#include <query.pb.h>
#include "Dispatcher.h"
#include "TcpConnection.h"

using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;
using google::protobuf::Message;
using reactor::proto::examples::Query;
using reactor::proto::examples::Answer;
using reactor::proto::Dispatcher;
using reactor::net::TcpConnection;
using reactor::core::Timestamp;
using google::protobuf::ShutdownProtobufLibrary;

void test1() {
    shared_ptr<Message> msg = make_shared<Query>();
    RC_DEBUG << "cnt: " << msg.use_count();
    shared_ptr<Query> query = dynamic_pointer_cast<Query>(msg);
    RC_DEBUG << "cnt: " << query.use_count();
    assert(msg);
    assert(query);
}

void on_unknown_type(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) {
    RC_WARN << "Unknown Message Type: " << msg->GetTypeName();
}

void on_query(const shared_ptr<TcpConnection> &con, const shared_ptr<Query> &msg, Timestamp ts) {
    RC_DEBUG << "On Query: " << msg->GetTypeName();
}

void on_answer(const shared_ptr<TcpConnection> &con, const shared_ptr<Answer> &msg, Timestamp ts) {
    RC_DEBUG << "On Answer: " << msg->GetTypeName();
}

int main(int argc, const char *argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

//    test1();
    Dispatcher dispatcher(on_unknown_type);

    dispatcher.register_message_callback<Query>(on_query);
    dispatcher.register_message_callback<Answer>(on_answer);

    shared_ptr<TcpConnection> con;
    Timestamp ts;

    shared_ptr<Query> query = make_shared<Query>();
    shared_ptr<Answer> answer = make_shared<Answer>();

    dispatcher.on_proto_message(con, query, ts);
    dispatcher.on_proto_message(con, answer, ts);

    ShutdownProtobufLibrary();
    return 0;
}
