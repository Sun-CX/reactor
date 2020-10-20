//
// Created by suncx on 2020/8/15.
//

#ifndef REACTOR_LOGSTREAM_H
#define REACTOR_LOGSTREAM_H

#include "FixedBuffer.h"
#include "String.h"
#include <algorithm>

using std::reverse;

const int small_buffer = 1024 * 4;              // 4KB
const int large_buffer = small_buffer * 1024;   // 4MB

class Formatter final : public NonCopyable {
private:
    char buf[32];
    int len;
public:
    template<class T>
    Formatter(const char *fmt, T x);

    [[nodiscard]]
    const char *data() const;

    [[nodiscard]]
    int length() const;
};

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

class SourceFile;

class LogStream final : public NonCopyable {
public:
    using Buffer = FixedBuffer<small_buffer>;
private:
    static const int max_size;
    Buffer buffer;

    /**
     * 数值格式化
     * @tparam T 数值类型
     * @param x 数值
     * @param format_type 格式化器
     */
    template<class T>
    void format_value(T x, size_t (*format_type)(char buf[], T x));

    friend LogStream &operator<<(LogStream &ls, const Formatter &fmt) {
        ls.append(fmt.data(), fmt.length());
        return ls;
    }

    void append(const char *data, size_t len);

public:

    [[nodiscard]]
    const Buffer &get_buffer() const;

    void reset_buffer();

    LogStream &operator<<(bool x);

    LogStream &operator<<(char x);

    LogStream &operator<<(signed char x);

    LogStream &operator<<(unsigned char x);

    LogStream &operator<<(short x);

    LogStream &operator<<(unsigned short x);

    LogStream &operator<<(int x);

    LogStream &operator<<(unsigned int x);

    LogStream &operator<<(long x);

    LogStream &operator<<(unsigned long x);

    LogStream &operator<<(long long x);

    LogStream &operator<<(unsigned long long x);

    LogStream &operator<<(float x);

    LogStream &operator<<(double x);

    LogStream &operator<<(const void *ptr);

    LogStream &operator<<(const char *s);

    LogStream &operator<<(const unsigned char *bytes);

    LogStream &operator<<(const string &str);

    LogStream &operator<<(const StringPiece &piece);

    LogStream &operator<<(const Buffer &buf);

    LogStream &operator<<(const String &str);

    LogStream &operator<<(const SourceFile &file);
};

#endif //REACTOR_LOGSTREAM_H
