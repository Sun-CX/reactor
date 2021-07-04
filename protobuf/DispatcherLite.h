//
// Created by 孙诚雄 on 2021/7/4.
//

#ifndef REACTOR_DISPATCHERLITE_H
#define REACTOR_DISPATCHERLITE_H

#include "NonCopyable.h"
#include "google/protobuf/message.h"
#include "Timestamp.h"
#include <functional>
#include <memory>
#include <map>

namespace reactor::net {
    class TcpConnection;
}

namespace reactor::proto {
    using reactor::core::NonCopyable;
    using std::function;
    using std::shared_ptr;
    using reactor::net::TcpConnection;
    using google::protobuf::Message;
    using google::protobuf::Descriptor;
    using reactor::core::Timestamp;
    using std::map;
    using std::move;

    class DispatcherLite final : public NonCopyable {
    public:
        using ProtobufMessageCallback = function<void(const shared_ptr<TcpConnection> &, const shared_ptr<Message> &, Timestamp)>;
    private:
        using CallbackMaps = map<const Descriptor *, ProtobufMessageCallback>;

        CallbackMaps callbacks;
        ProtobufMessageCallback callback;
    public:
        explicit DispatcherLite(ProtobufMessageCallback callback) : callbacks(), callback(move(callback)) {}

        void on_proto_message(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) const {
            auto it = callbacks.find(msg->GetDescriptor());
            if (it != callbacks.cend()) {
                it->second(con, msg, ts);
            } else {
                callback(con, msg, ts);
            }
        }

        void register_message_callback(const Descriptor *des, const ProtobufMessageCallback &call) {
            callbacks[des] = call;
        }
    };
}

#endif //REACTOR_DISPATCHERLITE_H