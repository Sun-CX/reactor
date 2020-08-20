//
// Created by suncx on 2020/8/16.
//

#ifndef REACTOR_LOGGER_H
#define REACTOR_LOGGER_H

#include "Timestamp.h"
#include "LogStream.h"

thread_local char errno_buf[512];
thread_local char time_an[64];
thread_local time_t last_second;

class Logger {
private:
    enum LogLevel {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    };

//    using LogLevel = Logger::LogLevel;

    class SourceFile {
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

        explicit SourceFile(const char *filename) : data(filename) {
            const char *slash = strrchr(filename, '/');
            if (slash != nullptr) {
                data = slash + 1;
            }
            size = strlen(data);
        }
    };

    class Impl {
    public:
        Timestamp timestamp;
        LogStream stream;
        LogLevel level;
        int line;
        SourceFile file;

        Impl(LogLevel lev, int old_no, const SourceFile &source, int line) : timestamp(Timestamp::now()), stream(),
                                                                             level(lev), line(line), file(source) {

        }

        void format_time() {
//            auto since_epoch = timestamp.
        }
    };

    Impl impl;
};


#endif //REACTOR_LOGGER_H
