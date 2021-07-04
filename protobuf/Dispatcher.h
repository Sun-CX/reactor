//
// Created by 孙诚雄 on 2021/7/4.
//

#ifndef REACTOR_DISPATCHER_H
#define REACTOR_DISPATCHER_H

#include "ConsoleStream.h"
#include "google/protobuf/message.h"
#include "Timestamp.h"
#include <map>
#include <memory>
#include <functional>

namespace reactor::net {
    class TcpConnection;
}

namespace reactor::proto {
    using reactor::core::NonCopyable;
    using std::map;
    using google::protobuf::Descriptor;
    using google::protobuf::Message;
    using std::shared_ptr;
    using std::make_shared;
    using std::dynamic_pointer_cast;
    using reactor::net::TcpConnection;
    using reactor::core::Timestamp;
    using std::is_base_of;
    using std::function;
    using std::move;

    class MessageCallback : public NonCopyable {
    public:
        virtual ~MessageCallback() = default;

        virtual void on_message(const shared_ptr<TcpConnection> &, const shared_ptr<Message> &, Timestamp) const = 0;
    };

    template<typename T>
    class ConcreteMessageCallback final : public MessageCallback {
        static_assert(is_base_of<Message, T>::value, "Must be derived from google::protobuf::Message!");
    private:
        friend class Dispatcher;

        using ConcreteProtobufMessageCallback = function<void(const shared_ptr<TcpConnection> &, const shared_ptr<T> &, Timestamp)>;
        ConcreteProtobufMessageCallback callback;
    public:
        explicit ConcreteMessageCallback(ConcreteProtobufMessageCallback callback) : callback(move(callback)) {}

        void on_message(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) const override {
            assert(msg != nullptr);
            callback(con, dynamic_pointer_cast<T>(msg), ts);
        }
    };

    class Dispatcher final : public NonCopyable {
    public:
        using ProtobufMessageCallback = function<void(const shared_ptr<TcpConnection> &, const shared_ptr<Message> &, Timestamp)>;
    private:
        using CallbackMaps =  map<const Descriptor *, shared_ptr<MessageCallback>>;

        CallbackMaps callbacks;
        ProtobufMessageCallback default_callback;

    public:
        explicit Dispatcher(ProtobufMessageCallback defaultCallback) : callbacks(), default_callback(move(defaultCallback)) {}

        template<typename T>
        void register_message_callback(const typename ConcreteMessageCallback<T>::ConcreteProtobufMessageCallback &callback) {
            callbacks[T::descriptor()] = make_shared<ConcreteMessageCallback<T>>(callback);
        }

        void on_proto_message(const shared_ptr<TcpConnection> &con, const shared_ptr<Message> &msg, Timestamp ts) const {
            auto it = callbacks.find(msg->GetDescriptor());

            if (it != callbacks.cend()) {
                it->second->on_message(con, msg, ts);
            } else {
                RC_WARN << "No Callback Found!";
                default_callback(con, msg, ts);
            }
        }
    };
}

#endif //REACTOR_DISPATCHER_H
