//
// Created by suncx on 2020/10/21.
//

#ifndef REACTOR_CONSOLESTREAM_H
#define REACTOR_CONSOLESTREAM_H

#include "NonCopyable.h"
#include <string>

#define NONE "\e[0m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"

using std::string;

class ConsoleStream final : public NonCopyable {
private:
    const char *style;
    const bool terminate;
public:
    explicit ConsoleStream(const char *style = "", bool terminate = false);

    virtual ~ConsoleStream();

    const ConsoleStream &operator<<(bool x) const;

    const ConsoleStream &operator<<(char x) const;

    const ConsoleStream &operator<<(signed char x) const;

    const ConsoleStream &operator<<(unsigned char x) const;

    const ConsoleStream &operator<<(short x) const;

    const ConsoleStream &operator<<(unsigned short x) const;

    const ConsoleStream &operator<<(int x) const;

    const ConsoleStream &operator<<(unsigned int x) const;

    const ConsoleStream &operator<<(long x) const;

    const ConsoleStream &operator<<(unsigned long x) const;

    const ConsoleStream &operator<<(long long x) const;

    const ConsoleStream &operator<<(unsigned long long x) const;

    const ConsoleStream &operator<<(float x) const;

    const ConsoleStream &operator<<(double x) const;

    const ConsoleStream &operator<<(const void *ptr) const;

    const ConsoleStream &operator<<(const char *s) const;

    const ConsoleStream &operator<<(const string &s) const;
};

#define LOG ConsoleStream()
#define INFO ConsoleStream(GREEN)
#define ERROR ConsoleStream(RED)
#define FATAL ConsoleStream(RED, true)
#endif //REACTOR_CONSOLESTREAM_H
