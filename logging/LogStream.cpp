//
// Created by suncx on 2020/8/15.
//

#include "LogStream.h"
#include "Logger.h"

const char Converter::digits[] = "9876543210123456789";
const char Converter::hex_digits[] = "0123456789ABCDEF";
const char *Converter::zero = Converter::digits + 9;
const int LogStream::max_size = 32;

template<class T>
void LogStream::format_value(T x, size_t (*format_type)(char *, T)) {
    if (buffer.available() >= max_size) {
        auto len = format_type(buffer.current(), x);
        buffer.add(len);
    }
}

void LogStream::append(const char *data, int len) {
    buffer.append(data, len);
}

const LogStream::Buffer &LogStream::get_buffer() const {
    return buffer;
}

void LogStream::reset_buffer() {
    buffer.reset();
}

LogStream &LogStream::operator<<(bool x) {
    if (x) buffer.append("true", 4);
    else buffer.append("false", 5);
    return *this;
}

LogStream &LogStream::operator<<(char x) {
    buffer.append(&x, 1);
    return *this;
}

LogStream &LogStream::operator<<(signed char x) {
    return *this << static_cast<char>(x);
}

LogStream &LogStream::operator<<(unsigned char x) {
    format_value<unsigned char>(x, [](char *buf, unsigned char x) -> size_t {
        buf[0] = '0';
        buf[1] = 'b';
        auto len = Converter::itoa_bin(buf + 2, x);
        return len + 2;
    });
    return *this;
}

LogStream &LogStream::operator<<(short x) {
    format_value<short>(x, [](char *buf, short x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(unsigned short x) {
    format_value<unsigned short>(x, [](char *buf, unsigned short x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(int x) {
    format_value<int>(x, [](char *buf, int x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(unsigned int x) {
    format_value<unsigned int>(x, [](char *buf, unsigned int x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(long x) {
    format_value<long>(x, [](char *buf, long x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(unsigned long x) {
    format_value<unsigned long>(x, [](char *buf, unsigned long x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(long long x) {
    format_value<long long>(x, [](char *buf, long long x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long x) {
    format_value<unsigned long long>(x, [](char *buf, unsigned long long x) -> size_t {
        return Converter::itoa_dec(buf, x);
    });
    return *this;
}

LogStream &LogStream::operator<<(float x) {
    format_value<float>(x, [](char *buf, float x) -> size_t {
        return snprintf(buf, max_size, "%.12g", x);
    });
    return *this;
}

LogStream &LogStream::operator<<(double x) {
    format_value<double>(x, [](char *buf, double x) -> size_t {
        return snprintf(buf, max_size, "%.12g", x);
    });
    return *this;
}

LogStream &LogStream::operator<<(const void *ptr) {
    auto p = reinterpret_cast<uintptr_t>(ptr);
    format_value<uintptr_t>(p, [](char *buf, uintptr_t pt) -> size_t {
        buf[0] = '0';
        buf[1] = 'x';
        auto len = Converter::itoa_hex(buf + 2, pt);
        return len + 2;
    });
    return *this;
}

LogStream &LogStream::operator<<(const char *str) {
    if (str == nullptr) {
        buffer.append("(null)", 6);
    } else {
        buffer.append(str, strlen(str));
    }
    return *this;
}

LogStream &LogStream::operator<<(const unsigned char *bytes) {
    return *this << reinterpret_cast<const char *>(bytes);
}

LogStream &LogStream::operator<<(const string &str) {
    buffer.append(str.c_str(), str.size());
    return *this;
}

LogStream &LogStream::operator<<(const StringPiece &piece) {
    buffer.append(piece.data(), piece.size());
    return *this;
}

LogStream &LogStream::operator<<(const LogStream::Buffer &buf) {
    return *this << buf.to_string_piece();
}

LogStream &LogStream::operator<<(const String &str) {
    append(str.get_data(), str.len());
    return *this;
}

LogStream &LogStream::operator<<(const SourceFile &file) {
    append(file.data, file.size);
    return *this;
}

template<class T>
Formatter::Formatter(const char *fmt, T val) {
    len = snprintf(buf, sizeof(buf), fmt, val);
}

const char *Formatter::data() const {
    return buf;
}

int Formatter::length() const {
    return len;
}

//template Formatter::Formatter(const char *fmt, char x);




