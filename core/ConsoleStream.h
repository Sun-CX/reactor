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

#define BUF_SIZE 2048

class ConsoleStream final : public NonCopyable {
private:

    static const char digits[];
    static const char hex_digits[];
    static const char *zero;

    template<typename T>
    size_t itoba(char *buf, T x) const;

    template<typename T>
    size_t itoda(char *buf, T x) const;

    template<typename T>
    size_t itoha(char *buf, T x) const;

    template<size_t N>
    class LogBuffer final {
    private:
        char buf[N];
        char *cur;

        size_t avail() const;

    public:
        LogBuffer();

        void append(const void *src, size_t len);

        const char *data() const;

        size_t size() const;
    };

    using Buffer = LogBuffer<BUF_SIZE>;

    Buffer buffer;
    const bool terminate;
public:
    explicit ConsoleStream(const char *src_file, int line, const char *style = "", bool terminate = false);

    virtual ~ConsoleStream();

    ConsoleStream &operator<<(bool x);

    ConsoleStream &operator<<(char x);

    ConsoleStream &operator<<(signed char x);

    ConsoleStream &operator<<(unsigned char x);

    ConsoleStream &operator<<(short x);

    ConsoleStream &operator<<(unsigned short x);

    ConsoleStream &operator<<(int x);

    ConsoleStream &operator<<(unsigned int x);

    ConsoleStream &operator<<(long x);

    ConsoleStream &operator<<(unsigned long x);

    ConsoleStream &operator<<(long long x);

    ConsoleStream &operator<<(unsigned long long x);

    ConsoleStream &operator<<(float x);

    ConsoleStream &operator<<(double x);

    ConsoleStream &operator<<(const void *ptr);

    ConsoleStream &operator<<(const char *s);

    ConsoleStream &operator<<(const string &s);
};

#define LOG ConsoleStream(__FILE__, __LINE__)
#define INFO ConsoleStream(__FILE__, __LINE__, GREEN)
#define ERROR ConsoleStream(__FILE__, __LINE__, RED)
#define FATAL ConsoleStream(__FILE__, __LINE__, RED, true)
#endif //REACTOR_CONSOLESTREAM_H
