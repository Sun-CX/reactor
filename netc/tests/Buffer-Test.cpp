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
    DEBUG << "first crlf index: " << idx - buf.begin() - buf.prepared_bytes();

    idx = buf.find_crlf(idx + 2);
    DEBUG << "second crlf index: " << idx - buf.begin() - buf.prepared_bytes();
}

int main(int argc, const char *argv[]) {
    test_find();
    return 0;
}