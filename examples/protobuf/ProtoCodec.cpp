//
// Created by 孙诚雄 on 2021/7/2.
//

#include "ProtoCodec.h"
#include "ConsoleStream.h"

using reactor::core::ConsoleStream;
using std::move;
using reactor::net::Buffer;

const int ProtoCodec::HEADER_LEN = sizeof(int32_t);
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
//        const int32_t len = in.
    }
}


