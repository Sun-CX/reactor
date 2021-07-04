//
// Created by 孙诚雄 on 2021/7/4.
//

#ifndef REACTOR_DISPATCHER_H
#define REACTOR_DISPATCHER_H

#include "NonCopyable.h"
#include "google/protobuf/message.h"
#include <map>
#include <memory>
#include <functional>
#include "Timestamp.h"
#include <cassert>
#include "ConsoleStream.h"

namespace reactor::net {
    class TcpConnection;
}

namespace reactor::proto {
    using reactor::core::NonCopyable;
    using std::map;
    using google::protobuf::Descriptor;
    using google::protobuf::Message;
    using std::shared_ptr;
    using reactor::net::TcpConnection;
    using reactor::core::Timestamp;
    using std::is_base_of;
    using std::function;
    using std::move;

    class Callback : public NonCopyable {
    public:
        virtual ~Callback() = default;

        virtual void on_message(const shared_ptr<TcpConnection> &, const shared_ptr<Message> &, Timestamp) const = 0;
    };

    template<typename T>
    class GenericCallback : public Callback {
        static_assert(is_base_of<Message, T>::value, "Must be derived from google::protobuf::Message");
    public:
        using ProtobufGenericMessageCallback = function<void(const shared_ptr<TcpConnection> &, const shared_ptr<Message> &, Timestamp)>;
    private:
        ProtobufGenericMessageCallback callback;
    public:
        explicit GenericCallback(ProtobufGenericMessageCallback callback) : callback(move(callback)) {}

        void on_message(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) const override {
            shared_ptr<T> concrete = msg;
            assert(concrete != nullptr);
            callback(con, concrete, ts);
        }
    };

    class Dispatcher final : public NonCopyable {
    public:
        using ProtobufMessageCallback = function<void(const shared_ptr<TcpConnection> &, const shared_ptr<Message> &, Timestamp)>;
    private:
        using CallbackMaps =  map<const Descriptor *, shared_ptr<Callback>>;

        CallbackMaps callbacks;
        ProtobufMessageCallback message_callback;

    public:
        template<typename T>
        void register_message_callback(const typename GenericCallback<T>::ProtobufGenericMessageCallback &callback) {
            shared_ptr<GenericCallback<T>> call(new GenericCallback<T>(callback));
            callbacks[T::descriptor()] = call;
        }

        void on_proto_message(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) const {
            auto it = callbacks.find(msg->GetDescriptor());

            if (it != callbacks.cend()) {
                it->second->on_message(con, msg, ts);
            } else {
                WARN << "NO Callback Found!";
                message_callback(con, msg, ts);
            }
        }
    };
}

#endif //REACTOR_DISPATCHER_H
