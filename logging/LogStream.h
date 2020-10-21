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
