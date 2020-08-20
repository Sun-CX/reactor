//
// Created by suncx on 2020/8/14.
//

#include <unistd.h>
#include <cstdio>
#include <csignal>
#include <cstdlib>
#include <cerrno>
#include "LogStream.h"

void test_signal() {
    signal(SIGTERM, [](int sig_no) {
        printf("receive sig_no: %d\n", sig_no);
        exit(errno);
    });
    printf("sleeping...\n");
    sleep(10 * 60);
}

void test1() {
    char str[32];
    auto l = Converter::itoa_dec(str, -120);
    printf("length: %lu, content: %s\n", l, str);
    l = Converter::itoa_hex(str, 16);
    printf("length: %lu, content: %s\n", l, str);
    l = Converter::itoa_bin(str, 32);
    printf("length: %lu, content: %s\n", l, str);
}

int main(int argc, const char *argv[]) {

//    test_signal();
    test1();

//    auto x = 0b10000;
//    printf("x = %d\n", x);


    return 0;
}