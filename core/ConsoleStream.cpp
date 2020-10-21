//
// Created by suncx on 2020/10/21.
//

#include "ConsoleStream.h"
#include "Converter.h"
#include "CurrentThread.h"
#include <cstdio>

ConsoleStream::ConsoleStream(const char *style, bool terminate) : style(style), terminate(terminate) {
    printf("%s%s[%d]: ", style, CurrentThread::name, CurrentThread::pid);
}

ConsoleStream::~ConsoleStream() {
    printf(NONE"\n");
    if (terminate) abort();
}

const ConsoleStream &ConsoleStream::operator<<(bool x) const {
    printf("%s", x ? "true" : "false");
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(char x) const {
    printf("%c", x);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(signed char x) const {
    return *this << static_cast<char>(x);
}

const ConsoleStream &ConsoleStream::operator<<(unsigned char x) const {
    char buf[16];
    buf[0] = '0';
    buf[1] = 'b';
    Converter::itoa_bin<unsigned char>(buf + 2, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(short x) const {
    char buf[8];
    Converter::itoa_dec<short>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(unsigned short x) const {
    char buf[8];
    Converter::itoa_dec<unsigned short>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(int x) const {
    char buf[16];
    Converter::itoa_dec<int>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(unsigned int x) const {
    char buf[16];
    Converter::itoa_dec<unsigned int>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(long x) const {
    char buf[32];
    Converter::itoa_dec<long>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(unsigned long x) const {
    char buf[32];
    Converter::itoa_dec<unsigned long>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(long long x) const {
    char buf[32];
    Converter::itoa_dec<long long>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(unsigned long long x) const {
    char buf[32];
    Converter::itoa_dec<unsigned long long>(buf, x);
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(float x) const {
    printf("%f", x);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(double x) const {
    printf("%f", x);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(const void *ptr) const {
    char buf[32];
    buf[0] = '0';
    buf[1] = 'x';
    Converter::itoa_hex<unsigned long>(buf + 2, reinterpret_cast<unsigned long>(ptr));
    printf("%s", buf);
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(const char *s) const {
    if (s == nullptr) {
        printf("(null)");
    } else {
        printf("%s", s);
    }
    return *this;
}

const ConsoleStream &ConsoleStream::operator<<(const string &s) const {
    return *this << s.c_str();
}
