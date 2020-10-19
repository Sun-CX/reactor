//
// Created by suncx on 2020/8/15.
//

#ifndef REACTOR_LOGSTREAM_H
#define REACTOR_LOGSTREAM_H

#include "NonCopyable.h"
#include "StringPiece.h"
#include <cstddef>
#include <string>
#include <algorithm>

using std::string;
using std::reverse;

const int small_buffer = 1024 * 4;              // 4KB
const int large_buffer = small_buffer * 1024;   // 4MB
using byte = unsigned char;

template<const int size>
class FixedBuffer final : public NonCopyable {
private:
    char data[size];
    char *cur;

    void (*cookie)();

    [[nodiscard]]
    const char *end() const {
        return data + sizeof(data);
    }

    static void cookie_start();

    static void cookie_end();

public:
    FixedBuffer() : cur(data) {
        set_cookie(cookie_start);
    }

    virtual ~FixedBuffer() {
        set_cookie(cookie_end);
    }

    void set_cookie(void (*c)()) {
        cookie = c;
    }

    void append(const char *buf, size_t len) {
        //TODO: append partially?
        if (available() > len) {
            memcpy(cur, buf, len);
            cur += len;
        }
    }

    [[nodiscard]]
    int available() const { return static_cast<int>(end() - cur); }

    [[nodiscard]]
    const char *get_data() const { return data; }

    [[nodiscard]]
    int length() const { return static_cast<int >(cur - data); }

    [[nodiscard]]
    char *current() const { return cur; }

    void add(size_t len) { cur += len; }

    void reset() { cur = data; }

    void zero() { memset(data, 0, sizeof(data)); }

    //for gdb used only!
    const char *debug_data();

    [[nodiscard]]
    string to_string() const { return string(data, length()); }

    [[nodiscard]]
    StringPiece to_string_piece() const {
        return StringPiece(data, length());
    }
};

template<const int size>
void FixedBuffer<size>::cookie_start() {}

template<const int size>
void FixedBuffer<size>::cookie_end() {}

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

class LogStream : public NonCopyable {
private:
    using Buffer = FixedBuffer<small_buffer>;

    Buffer buffer;
    static const int max_size = 32;

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

public:

    void append(const char *data, int len);

    [[nodiscard]]
    const Buffer &get_buffer() const;

    void reset_buffer();

    LogStream &operator<<(bool x);

    LogStream &operator<<(char x);

    LogStream &operator<<(signed char x);

    LogStream &operator<<(byte x);

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

    LogStream &operator<<(const char *str);

    LogStream &operator<<(const byte *bytes);

    LogStream &operator<<(const string &str);

    LogStream &operator<<(const StringPiece &piece);

    LogStream &operator<<(const Buffer &buf);

};

#endif //REACTOR_LOGSTREAM_H
