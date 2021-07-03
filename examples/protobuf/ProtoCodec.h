//
// Created by 孙诚雄 on 2021/7/2.
//

#ifndef REACTOR_PROTOCODEC_H
#define REACTOR_PROTOCODEC_H

#include "NonCopyable.h"
#include "Timestamp.h"
#include "TcpConnection.h"
#include <functional>
#include <memory>
#include "google/protobuf/message.h"

using reactor::core::NonCopyable;
using reactor::core::Timestamp;
using reactor::net::TcpConnection;
using std::function;
using std::shared_ptr;
using google::protobuf::Message;
using reactor::net::byte;

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
    static const int HEADER_LEN;
    static const int MIN_MESSAGE_LEN;
    static const int MAX_MESSAGE_LEN;

    static void default_error_callback(const shared_ptr<TcpConnection> &con, Timestamp s, ErrorCode rc);

    ProtobufMessageCallback message_callback;
    ErrorCallback error_callback;

public:
    static shared_ptr<Message> parse(const byte *buf, size_t len, ErrorCode &ec);

    explicit ProtoCodec(ProtobufMessageCallback messageCallback, ErrorCallback errorCallback = default_error_callback);

    void on_message(const shared_ptr<TcpConnection> &con, Timestamp ts);
};

#endif //REACTOR_PROTOCODEC_H
