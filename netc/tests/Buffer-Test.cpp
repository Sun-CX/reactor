//
// Created by suncx on 2020/8/19.
//

#include "Buffer.h"
#include "ConsoleStream.h"

using reactor::net::Buffer;

static void test_find() {
    Buffer buf;

    char msg[] = "hello\r\nworld\r\n.";

    buf.append(msg, sizeof(msg));

    auto idx = buf.find_crlf();
    DEBUG << "first crlf index: " << idx - buf.begin() - buf.reserved_bytes();

    idx = buf.find_crlf(idx + 2);
    DEBUG << "second crlf index: " << idx - buf.begin() - buf.reserved_bytes();
}

void test() {
    Buffer buffer(8);
    buffer.append("hello");

//    cout << buffer.retrieve_all_string() << endl;

    buffer.append("world");
//    cout << buffer.retrieve_all_string() << endl;

    buffer.retrieve(8);

    buffer.append("fuck");
    DEBUG << buffer.retrieve_all_string();
}

int main(int argc, const char *argv[]) {

//    test_find();
    test();
    return 0;
}