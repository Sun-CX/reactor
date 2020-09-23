//
// Created by suncx on 2020/8/19.
//

#include <Buffer.h>

static void test_find() {
    Buffer buf;
    char msg[] = "hello\r\nworld\r\n.";

    buf.append(msg, sizeof(msg));

    auto idx = buf.find_CRLF();

    printf("idx: %ld\n", idx - buf.begin());

    idx = buf.find_CRLF(idx + 2);
    printf("idx: %ld\n", idx - buf.begin());
}

int main(int argc, const char *argv[]) {

    test_find();
    return 0;
}