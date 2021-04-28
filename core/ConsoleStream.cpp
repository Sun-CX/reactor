//
// Created by suncx on 2020/10/21.
//

#include "ConsoleStream.h"
#include "Thread.h"
#include "Timestamp.h"
#include <cstring>
#include <algorithm>

using std::reverse;

const char ConsoleStream::digits[] = "9876543210123456789";
const char ConsoleStream::hex_digits[] = "0123456789abcdef";
const char *ConsoleStream::zero = digits + 9;

template<typename T>
size_t ConsoleStream::itoba(char *buf, T x) const {
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

template<typename T>
size_t ConsoleStream::itoda(char *buf, T x) const {
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

template<typename T>
size_t ConsoleStream::itoha(char *buf, T x) const {
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

template<size_t N>
ConsoleStream::LogBuffer<N>::LogBuffer():cur(buf) {}

template<size_t N>
size_t ConsoleStream::LogBuffer<N>::avail() const {
    return buf + N - cur;
}

template<size_t N>
void ConsoleStream::LogBuffer<N>::append(const void *src, size_t len) {
    if (len <= avail()) {
        memcpy(cur, src, len);
        cur += len;
    } else {
        fprintf(stderr, "LogBuffer has been full, log is incomplete.\n");
    }
}

template<size_t N>
const char *ConsoleStream::LogBuffer<N>::data() const {
    return buf;
}

template<size_t N>
size_t ConsoleStream::LogBuffer<N>::size() const {
    return cur - buf;
}

ConsoleStream::ConsoleStream(const char *src_file, int line, const char *style, bool terminate)
        : buffer(), terminate(terminate) {
    *this << style << Timestamp::now().to_string();

    *this << " [" << CurrentThread::name << "] [";

    *this << src_file << ':' << line << "] ";
}

ConsoleStream::~ConsoleStream() {
    *this << STYLE_RESET"\n";
    fwrite(buffer.data(), 1, buffer.size(), stdout);
    if (terminate) {
        fflush(stdout);
        abort();
    }
}

ConsoleStream &ConsoleStream::operator<<(bool x) {
    if (x) {
        buffer.append("true", 4);
    } else {
        buffer.append("false", 5);
    }
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(char x) {
    buffer.append(&x, 1);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(signed char x) {
    return *this << static_cast<char>(x);
}

ConsoleStream &ConsoleStream::operator<<(unsigned char x) {
    char buf[16];
    buf[0] = '0';
    buf[1] = 'x';

    size_t n = itoha<unsigned char>(buf + 2, x);
    buffer.append(buf, n + 2);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(short x) {
    char buf[16];

    size_t n = itoda<short>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(unsigned short x) {
    char buf[16];

    size_t n = itoda<unsigned short>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(int x) {
    char buf[16];
    size_t n = itoda<int>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(unsigned int x) {
    char buf[16];
    size_t n = itoda<unsigned int>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(long x) {
    char buf[32];
    size_t n = itoda<long>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(unsigned long x) {
    char buf[32];
    size_t n = itoda<unsigned long>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(long long x) {
    char buf[32];
    size_t n = itoda<long long>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(unsigned long long x) {
    char buf[32];
    size_t n = itoda<unsigned long long>(buf, x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(float x) {
    return *this << static_cast<double>(x);
}

ConsoleStream &ConsoleStream::operator<<(double x) {
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%.2g", x);
    buffer.append(buf, n);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(const void *ptr) {
    char buf[32];
    buf[0] = '0';
    buf[1] = 'x';

    size_t n = itoha<uintptr_t>(buf + 2, reinterpret_cast<uintptr_t>(ptr));
    buffer.append(buf, n + 2);
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(const char *s) {
    if (s == nullptr) {
        buffer.append("(null)", 6);
    } else {
        buffer.append(s, strlen(s));
    }
    return *this;
}

ConsoleStream &ConsoleStream::operator<<(const string &s) {
    return *this << s.c_str();
}
