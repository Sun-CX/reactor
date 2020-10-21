//
// Created by suncx on 2020/10/21.
//

#ifndef REACTOR_CONVERTER_H
#define REACTOR_CONVERTER_H

#include "NonCopyable.h"
#include <cstddef>
#include <algorithm>
#include <cstdio>

using std::reverse;

class Converter final : public NonCopyable {
private:
    static const char digits[];
    static const char hex_digits[];
    static const char *zero;
public:
    /**
     * 将整数 x 转换为十进制字符串
     * @param buf
     * @param x
     * @return 字符串长度
     */
    template<class T>
    static size_t itoa_dec(char *buf, T x) {
        decltype(x) i = x;
        char *p = buf;
        decltype(i) lsd;
        do {
            lsd = i % 10;
            i /= 10;
            *p++ = zero[lsd];
        } while (i != 0);
        if (x < 0) *p++ = '-';
        *p = '\0';
        reverse(buf, p);
        return p - buf;
    }

    /**
     * 将整数 x 转换为十六进制字符串
     * @param buf
     * @param x
     * @return 字符串长度
     */
    template<class T>
    static size_t itoa_hex(char *buf, T x) {
        decltype(x) i = x;
        char *p = buf;
        decltype(i) lsd;
        do {
            lsd = i % 16;
            i /= 16;
            *p++ = hex_digits[lsd];
        } while (i != 0);
        *p = '\0';
        reverse(buf, p);
        return p - buf;
    }

    /**
     * 将整数 x 转换为二进制字符串
     * @param buf
     * @param x
     * @return 字符串长度
     */
    template<class T>
    static size_t itoa_bin(char *buf, T x) {
        decltype(x) i = x;
        char *p = buf;
        decltype(i) lsd;
        do {
            lsd = i % 2;
            i /= 2;
            *p++ = hex_digits[lsd];
        } while (i != 0);
        *p = '\0';
        reverse(buf, p);
        return p - buf;
    }
};

const char Converter::digits[] = "9876543210123456789";
const char Converter::hex_digits[] = "0123456789abcdef";
const char *Converter::zero = Converter::digits + 9;
#endif //REACTOR_CONVERTER_H
