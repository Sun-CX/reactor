//
// Created by 孙诚雄 on 2021/7/3.
//

#include "google/protobuf/message.h"
#include "query.pb.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;
using reactor::net::examples::Query;
using google::protobuf::Descriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::Message;
using google::protobuf::MessageFactory;

static void assert_test() {
    const string &type_name = Query::descriptor()->full_name();
    cout << type_name << endl;

    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    assert(descriptor == Query::descriptor());
    cout << "FindMessageTypeByName() = " << descriptor << endl;
    cout << "Query::descriptor() = " << Query::descriptor() << endl;
    cout << endl;

    const Message *prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
    assert(prototype == &Query::default_instance());
    cout << "GetPrototype() = " << prototype << endl;
    cout << "Query::default_instance() = " << &Query::default_instance();
    cout << endl;

    auto new_obj = dynamic_cast<Query *>(prototype->New());
    assert(new_obj != nullptr);
    assert(new_obj != prototype);
    assert(typeid(*new_obj) == typeid(Query::default_instance()));
    cout << "prototype->New() = " << new_obj << endl;
    cout << endl;

    delete new_obj;
}

static Message *create_message(const string &type_name) {
    Message *message = nullptr;
    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    if (descriptor) {
        const Message *prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        }
    }
    return message;
}

int main(int argc, const char *argv[]) {

//    assert_test();
    Message *new_query = create_message("reactor.net.examples.Query");
    assert(new_query != nullptr);
    assert(typeid(*new_query) == typeid(Query::default_instance()));
    cout << "create message: " << new_query << endl;
    delete new_query;

    return 0;
}
