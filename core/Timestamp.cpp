//
// Created by suncx on 2020/8/8.
//

#include "Timestamp.h"
#include "Ext.h"
#include "ConsoleStream.h"
#include <cstring>
#include <cassert>

using std::string;
using reactor::core::Timestamp;

Timestamp::Timestamp() : microseconds_since_epoch(0) {}

Timestamp::Timestamp(int64_t microseconds) : microseconds_since_epoch(microseconds) {
    assert(microseconds_since_epoch >= 0);
}

Timestamp &Timestamp::operator+(const Timestamp &ts) {
    microseconds_since_epoch += ts.microseconds_since_epoch;
    return *this;
}

Timestamp Timestamp::operator+(const Timestamp &ts) const {
    return Timestamp(microseconds_since_epoch + ts.microseconds_since_epoch);
}

Timestamp &Timestamp::operator-(const Timestamp &ts) {
    assert(microseconds_since_epoch >= ts.microseconds_since_epoch);
    microseconds_since_epoch -= ts.microseconds_since_epoch;
    return *this;
}

Timestamp Timestamp::operator-(const Timestamp &ts) const {
    return Timestamp(microseconds_since_epoch - ts.microseconds_since_epoch);
}

bool Timestamp::operator<(const Timestamp &ts) const {
    return microseconds_since_epoch < ts.microseconds_since_epoch;
}

bool Timestamp::operator>(const Timestamp &ts) const {
    return ts < *this;
}

bool Timestamp::operator<=(const Timestamp &ts) const {
    return !(ts < *this);
}

bool Timestamp::operator>=(const Timestamp &ts) const {
    return !(*this < ts);
}

bool Timestamp::operator==(const Timestamp &ts) const {
    return microseconds_since_epoch == ts.microseconds_since_epoch;
}

bool Timestamp::operator!=(const Timestamp &ts) const {
    return !(ts == *this);
}

Timestamp Timestamp::now() {
    timespec ts;
    if (unlikely(::clock_gettime(CLOCK_REALTIME, &ts) < 0))
        RC_FATAL << "clock get time error: " << ::strerror(errno);
    return Timestamp(ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

string Timestamp::to_string(bool show_ms) const {
    char buf[32];
    time_t sec = microseconds_since_epoch / 1000000;

    tm tm_time;

    gmtime_r(&sec, &tm_time);

    if (show_ms) {
        int64_t ms = microseconds_since_epoch % 1000000 / 1000;
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
                 tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1,
                 tm_time.tm_mday,
                 tm_time.tm_hour,
                 tm_time.tm_min,
                 tm_time.tm_sec,
                 ms);
        return buf;
    }

    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900,
             tm_time.tm_mon + 1,
             tm_time.tm_mday,
             tm_time.tm_hour,
             tm_time.tm_min,
             tm_time.tm_sec);
    return buf;
}

int64_t Timestamp::time_since_epoch() const {
    return microseconds_since_epoch;
}

timespec Timestamp::to_timespec() const {
    timespec ts;
    ts.tv_sec = microseconds_since_epoch / 1000000;
    ts.tv_nsec = microseconds_since_epoch % 1000000 * 1000;
    return ts;
}