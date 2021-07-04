//
// Created by 孙诚雄 on 2021/7/4.
//

#include "DispatcherLite.h"
#include "ConsoleStream.h"
#include "TcpConnection.h"
#include "query.pb.h"
#include <cassert>

using reactor::proto::DispatcherLite;
using reactor::net::TcpConnection;
using reactor::core::Timestamp;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using google::protobuf::Message;
using google::protobuf::ShutdownProtobufLibrary;
using reactor::proto::examples::Query;
using reactor::proto::examples::Answer;

void unknown_message_type(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) {
    RC_WARN << "UNKNOWN MESSAGE TYPE: " << msg->GetTypeName();
}

void on_query(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) {
    RC_DEBUG << "on_query: " << msg->GetTypeName();
    shared_ptr<Query> query = dynamic_pointer_cast<Query>(msg);
    assert(query != nullptr);
}

void on_answer(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) {
    RC_DEBUG << "on_answer: " << msg->GetTypeName();
    shared_ptr<Answer> answer = dynamic_pointer_cast<Answer>(msg);
    assert(answer != nullptr);
}

int main(int argc, const char *argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    DispatcherLite dispatcher(unknown_message_type);

    dispatcher.register_message_callback(Query::descriptor(), on_query);
    dispatcher.register_message_callback(Answer::descriptor(), on_answer);

    shared_ptr<TcpConnection> conn;
    Timestamp ts;

    shared_ptr<Query> query(new Query);
    shared_ptr<Answer> answer(new Answer);

    dispatcher.on_proto_message(conn, query, ts);
    dispatcher.on_proto_message(conn, answer, ts);

    ShutdownProtobufLibrary();
    return 0;
}