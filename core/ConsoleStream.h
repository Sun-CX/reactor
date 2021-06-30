//
// Created by suncx on 2020/10/21.
//

#ifndef REACTOR_CONSOLESTREAM_H
#define REACTOR_CONSOLESTREAM_H

#include "NonCopyable.h"
#include <string>

using std::string;

#define BUF_SIZE 2048

namespace reactor::core {
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

            [[nodiscard]]
            size_t avail() const;

        public:
            LogBuffer();

            void append(const void *src, size_t len);

            [[nodiscard]]
            const char *data() const;

            [[nodiscard]]
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
}

/* SGR style def */
#define STYLE_RESET "\e[0m"
#define STYLE_TRACE "\e[38;5;252m"
#define STYLE_DEBUG "\e[1;38;5;252m"
#define STYLE_INFO "\e[38;5;44m"
#define STYLE_WARN "\e[38;5;190m"
#define STYLE_ERROR "\e[38;5;160m"
#define STYLE_FATAL "\e[1;38;5;160m"

#define TRACE reactor::core::ConsoleStream(__FILE__, __LINE__, STYLE_TRACE)
#define DEBUG reactor::core::ConsoleStream(__FILE__, __LINE__, STYLE_DEBUG)
#define INFO reactor::core::ConsoleStream(__FILE__, __LINE__, STYLE_INFO)
#define WARN reactor::core::ConsoleStream(__FILE__, __LINE__, STYLE_WARN)
#define ERROR reactor::core::ConsoleStream(__FILE__, __LINE__, STYLE_ERROR)
#define FATAL reactor::core::ConsoleStream(__FILE__, __LINE__, STYLE_FATAL, true)

#endif //REACTOR_CONSOLESTREAM_H
