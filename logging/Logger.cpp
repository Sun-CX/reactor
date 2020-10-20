//
// Created by suncx on 2020/8/16.
//

#include "Logger.h"

const char *Logger::LogLevelName[6] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

Logger::Logger(SourceFile file, int line) : impl(INFO, 0, file, line) {}

Logger::Logger(SourceFile file, int line, Logger::LogLevel level) : impl(level, 0, file, line) {}

Logger::Logger(SourceFile file, int line, Logger::LogLevel level, const char *func) : impl(level, 0, file, line) {
    impl.stream << func << ": ";
}

Logger::Logger(SourceFile file, int line, bool abort) : impl(abort ? FATAL : ERROR, errno, file, line) {}

Logger::~Logger() {
    impl.finish();
    const LogStream::Buffer &buf = stream().get_buffer();
    auto n = fwrite(buf.get_data(), 1, buf.length(), stdout);
    if (impl.level == FATAL) {
        fflush(stdout);
        abort();
    }
}

LogStream &Logger::stream() {
    return impl.stream;
}

Logger::LogLevel G_LOG_LEVEL = init_logger_level();

Logger::LogLevel init_logger_level() noexcept {
    if (getenv("REACTOR_LOG_TRACE") != nullptr) {
        return Logger::LogLevel::TRACE;
    } else if (getenv("REACTOR_LOG_DEBUG") != nullptr) {
        return Logger::LogLevel::DEBUG;
    } else return Logger::LogLevel::INFO;
}