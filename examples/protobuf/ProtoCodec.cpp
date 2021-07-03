//
// Created by 孙诚雄 on 2021/7/2.
//

#include "ProtoCodec.h"
#include "ConsoleStream.h"

using reactor::core::ConsoleStream;
using std::move;
using reactor::net::Buffer;

const int ProtoCodec::HEADER_LEN = sizeof(uint32_t);
const int ProtoCodec::MIN_MESSAGE_LEN = 2 * HEADER_LEN + 2;
const int ProtoCodec::MAX_MESSAGE_LEN = 64 * 1024 * 1024;

void ProtoCodec::default_error_callback(const shared_ptr<TcpConnection> &con, Timestamp s, ErrorCode rc) {
    ERROR << __PRETTY_FUNCTION__ << " invoked error with errno = " << rc;
    if (con) con->shutdown();
}

ProtoCodec::ProtoCodec(ProtoCodec::ProtobufMessageCallback messageCallback, ProtoCodec::ErrorCallback errorCallback) : message_callback(move(messageCallback)), error_callback(move(errorCallback)) {}

void ProtoCodec::on_message(const shared_ptr<TcpConnection> &con, Timestamp ts) {
    Buffer &in = con->inbound_buf();
    while (in.readable_bytes() >= MIN_MESSAGE_LEN + HEADER_LEN) {
        const auto len = in.peek_uint32();
        if (len > MAX_MESSAGE_LEN or len < MIN_MESSAGE_LEN) {
            error_callback(con, ts, INVALID_LENGTH);
            break;
        } else if (in.readable_bytes() >= len + HEADER_LEN) {
            ErrorCode ec = NO_ERROR;
            shared_ptr<Message> mep = parse(in.peek() + HEADER_LEN, len, ec);
            if (ec == NO_ERROR and mep) {
                message_callback(con, mep, ts);
                in.retrieve(HEADER_LEN + len);
            } else {
                error_callback(con, ts, ec);
                break;
            }
        } else break;
    }
}

shared_ptr<Message> ProtoCodec::parse(const byte *buf, size_t len, ProtoCodec::ErrorCode &ec) {
    shared_ptr<Message> ptr;


    return shared_ptr<Message>();
}


