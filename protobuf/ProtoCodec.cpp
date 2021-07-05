//
// Created by 孙诚雄 on 2021/7/2.
//

#include "ProtoCodec.h"
#include "google/protobuf/message.h"
#include "ConsoleStream.h"
#include "zlib.h"

using reactor::proto::ProtoCodec;
using reactor::core::ConsoleStream;
using std::move;
using reactor::net::Buffer;
using std::shared_ptr;
using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::MessageFactory;

const uint32_t ProtoCodec::MIN_TYPE_NAME_LEN = 2u;
const uint32_t ProtoCodec::MIN_DATA_LEN = sizeof(uint32_t) + sizeof(uint32_t) + MIN_TYPE_NAME_LEN + sizeof(uint32_t);

void ProtoCodec::default_error_callback(const shared_ptr<TcpConnection> &con, Timestamp s, ErrorCode rc) {
    RC_ERROR << __PRETTY_FUNCTION__ << " invoked error with errno = " << rc;
    if (con) con->shutdown();
}

uint32_t ProtoCodec::as_uint32_t(const byte *buf) {
    uint32_t val;
    ::memcpy(&val, buf, sizeof(val));
    return ntohl(val);
}

ProtoCodec::ProtoCodec(ProtoCodec::ProtobufMessageCallback messageCallback, ProtoCodec::ErrorCallback errorCallback) :
        message_callback(move(messageCallback)),
        error_callback(move(errorCallback)) {}

void ProtoCodec::on_message(const shared_ptr<TcpConnection> &con, Timestamp ts) {
    Buffer &in = con->inbound_buf();
    while (in.readable_bytes() >= MIN_DATA_LEN) {
        auto len = in.peek_uint32();

        /* check range of len */
        if (len < sizeof(uint32_t) + MIN_TYPE_NAME_LEN + sizeof(uint32_t)) {
            error_callback(con, ts, INVALID_LENGTH);
            break;
        }

        if (in.readable_bytes() >= sizeof(len) + len) {
            ErrorCode ec = NO_ERROR;
            shared_ptr<Message> message = parse_body(in.peek() + sizeof(len), len, ec);
            if (ec == NO_ERROR and message) {
                message_callback(con, message, ts);
                in.retrieve(sizeof(len) + len);
                continue;
            } else {
                error_callback(con, ts, ec);
                break;
            }
        }

        error_callback(con, ts, INVALID_LENGTH);
        break;
    }
}

shared_ptr<Message> ProtoCodec::parse_body(const byte *payload, size_t len, ProtoCodec::ErrorCode &ec) {
    shared_ptr<Message> message;

    uint32_t expected_checksum = as_uint32_t(payload + len - sizeof(uint32_t));
    uint32_t checksum = ::adler32(1u, payload, len - sizeof(uint32_t));

    if (checksum == expected_checksum) {

        auto name_len = as_uint32_t(payload);

        /* name_len range check */
        if (name_len >= 2 and name_len <= len - sizeof(name_len) - sizeof(checksum)) {

            string type_name(payload + sizeof(name_len), payload + sizeof(name_len) + name_len);

            message.reset(create_message(type_name));

            if (message) {
                const byte *message_data = payload + sizeof(name_len) + name_len;
                uint32_t data_len = len - sizeof(name_len) - name_len - sizeof(checksum);
                if (message->ParseFromArray(message_data, data_len)) {
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

Message *ProtoCodec::create_message(const string &type_name) {
    Message *message = nullptr;

    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);

    if (descriptor) {
        const Message *prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype)
            message = prototype->New();
    }

    return message;
}

void ProtoCodec::send_message(const shared_ptr<TcpConnection> &con, const Message &message) const {
    Buffer &out = con->outbound_buf();
    assert(out.readable_bytes() == 0);

    const string &type_name = message.GetTypeName();

    size_t name_len = type_name.length() + 1u;

    out.append_uint32(name_len);
    out.append(type_name.c_str(), name_len);

    size_t byte_size = message.ByteSizeLong();

    out.ensure_writable(byte_size);
    byte *end = message.SerializeWithCachedSizesToArray(out.begin_write());
    out.has_written(byte_size);

    auto checksum = ::adler32(1, out.peek(), out.readable_bytes());
    out.append_uint32(checksum);

    size_t len = htonl(out.readable_bytes());

    out.prepend(&len, 4);

    con->send_outbound_bytes();
}