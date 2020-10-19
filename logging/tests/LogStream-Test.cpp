//
// Created by suncx on 2020/8/14.
//

#include "LogStream.h"

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

    test1();
    
    return 0;
}