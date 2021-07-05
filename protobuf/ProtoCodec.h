//
// Created by 孙诚雄 on 2021/7/2.
//
// 消息传输格式定义：
//
// struct ProtobufTransportFormat {
//     uint32_t len;
//     uint32_t name_len;
//     char type_name[name_len];
//     byte protobuf_data[len - sizeof(name_len) - name_len - sizeof(checksum)];
//     uint32_t checksum;
// };
//
// 其中：
// 规定 name_len 最小值为 2，即 name_len >= 2
// 当 name_len = 2 且 protobuf_data 长度为 0 时，len 取得最小值 14，故 len >= 14
// checksum 为 name_len ~ protobuf_data 部分
//

#ifndef REACTOR_PROTOCODEC_H
#define REACTOR_PROTOCODEC_H

#include "TcpConnection.h"

namespace google::protobuf {
    class Message;
}

namespace reactor::core {
    class Timestamp;
}

namespace reactor::proto {
    using reactor::core::NonCopyable;
    using reactor::core::Timestamp;
    using reactor::net::TcpConnection;
    using std::function;
    using std::shared_ptr;
    using google::protobuf::Message;
    using reactor::net::byte;
    using std::string;

    class ProtoCodec final : public NonCopyable {
    public:
        enum ErrorCode {
            NO_ERROR,
            INVALID_LENGTH,
            CHECKSUM_ERROR,
            INVALID_NAME_LEN,
            UNKNOWN_MESSAGE_TYPE,
            PARSE_ERROR
        };
        using ProtobufMessageCallback = function<void(const shared_ptr<TcpConnection> &, const shared_ptr<Message> &, Timestamp)>;
        using ErrorCallback = function<void(const shared_ptr<TcpConnection> &, Timestamp, ErrorCode)>;
    private:
        static const uint32_t MIN_TYPE_NAME_LEN;

        static const uint32_t MIN_DATA_LEN;

        static void default_error_callback(const shared_ptr<TcpConnection> &con, Timestamp s, ErrorCode rc);

        static uint32_t as_uint32_t(const byte *buf);

        ProtobufMessageCallback message_callback;
        ErrorCallback error_callback;

    public:
        static shared_ptr<Message> parse_body(const byte *payload, size_t len, ErrorCode &ec);

        static Message *create_message(const string &type_name);

        explicit ProtoCodec(ProtobufMessageCallback messageCallback, ErrorCallback errorCallback = default_error_callback);

        void on_message(const shared_ptr<TcpConnection> &con, Timestamp ts);

        void send_message(const shared_ptr<TcpConnection> &con, const Message &message) const;
    };
}

#endif //REACTOR_PROTOCODEC_H
