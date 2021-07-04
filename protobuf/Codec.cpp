//
// Created by 孙诚雄 on 2021/7/2.
//

#include "Codec.h"
#include "google/protobuf/message.h"
#include "ConsoleStream.h"
#include "zlib.h"

using reactor::proto::Codec;
using reactor::core::ConsoleStream;
using std::move;
using reactor::net::Buffer;
using std::shared_ptr;
using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::MessageFactory;

const int Codec::HEADER_LEN = sizeof(uint32_t);
const int Codec::MIN_MESSAGE_LEN = 2 * HEADER_LEN + 2;
const int Codec::MAX_MESSAGE_LEN = 64 * 1024 * 1024;

void Codec::default_error_callback(const shared_ptr<TcpConnection> &con, Timestamp s, ErrorCode rc) {
    RC_ERROR << __PRETTY_FUNCTION__ << " invoked error with errno = " << rc;
    if (con) con->shutdown();
}

uint32_t Codec::as_uint32_t(const byte *buf) {
    uint32_t val;
    ::memcpy(&val, buf, sizeof(val));
    return ntohl(val);
}

Codec::Codec(Codec::ProtobufMessageCallback messageCallback, Codec::ErrorCallback errorCallback) : message_callback(move(messageCallback)), error_callback(move(errorCallback)) {}

void Codec::on_message(const shared_ptr<TcpConnection> &con, Timestamp ts) {
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

shared_ptr<Message> Codec::parse(const byte *payload, size_t len, Codec::ErrorCode &ec) {
    shared_ptr<Message> message;

    uint32_t expected_checksum = as_uint32_t(payload + len - HEADER_LEN);
    uint32_t checksum = ::adler32(1, payload, len - HEADER_LEN);

    if (checksum == expected_checksum) {

        auto name_len = as_uint32_t(payload);
        if (name_len >= 2 and name_len <= len - 2 * HEADER_LEN) {

            string type_name(payload + HEADER_LEN, payload + HEADER_LEN + name_len - 1);

            message.reset(create_message(type_name));

            if (message) {
                const byte *data = payload + HEADER_LEN + name_len;
                uint32_t data_len = len - name_len - 2 * HEADER_LEN;
                if (message->ParseFromArray(data, data_len)) {
                    ec = NO_ERROR;
                } else ec = PARSE_ERROR;
            } else {
                RC_ERROR << "UnknownMessageType";
            }
        } else {
            RC_ERROR << "Invalid name length";
        }
    } else {
        ec = CHECKSUM_ERROR;
    }
    return message;
}

Message *Codec::create_message(const string &type_name) {
    Message *message = nullptr;

    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);

    if (descriptor) {
        const Message *prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype)
            message = prototype->New();
    }

    return message;
}
