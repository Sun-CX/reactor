//
// Created by suncx on 2020/8/16.
//

#ifndef REACTOR_LOGGER_H
#define REACTOR_LOGGER_H

#include "CurrentThread.h"
#include "Timestamp.h"
#include "LogStream.h"
#include "String.h"

class SourceFile final {
public:
    const char *data;
    size_t size;

    template<size_t n>
    explicit SourceFile(const char (&arr)[n]):data(arr), size(n - 1) {
        const char *slash = strrchr(data, '/');
        if (slash != nullptr) {
            data = slash + 1;
            size -= data - arr;
        }
    }

    SourceFile(const char *filename) : data(filename) {
        const char *slash = strrchr(filename, '/');
        if (slash != nullptr) data = slash + 1;
        size = strlen(data);
    }
};

class Logger {
public:
    enum LogLevel {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    };
private:
    class Impl final {
    public:
        Timestamp time;
        LogStream stream;
        LogLevel level;
        int line;
        SourceFile file;

        Impl(LogLevel lev, int old_no, const SourceFile &source, int line) : time(Timestamp::now()), stream(),
                                                                             level(lev), line(line), file(source) {
            format_time();
            stream << '[' << String(CurrentThread::name, strlen(CurrentThread::name)) << ']';
            stream << ' ' << String(Logger::LogLevelName[lev], 5) << ' ';
        }

        void format_time() {
//            auto since_epoch = timestamp.
        }

        void finish() {
            stream << " - " << file << ':' << line << '\n';
        }
    };

    static const char *LogLevelName[6];

    Impl impl;
public:
    using output_func = void (*)(const char *msg, int len);
    using flush_func = void (*)();

    Logger(SourceFile file, int line);

    Logger(SourceFile file, int line, LogLevel level);

    Logger(SourceFile file, int line, LogLevel level, const char *func);

    Logger(SourceFile file, int line, bool abort);

    virtual ~Logger();

    LogStream &stream();
};

extern Logger::LogLevel G_LOG_LEVEL;

extern Logger::LogLevel init_logger_level() noexcept;

#define LOG_TRACE if(G_LOG_LEVEL <= Logger::LogLevel::INFO)\
Logger(__FILE__, __LINE__, Logger::TRACE, __func__ ).stream()

#endif //REACTOR_LOGGER_H
