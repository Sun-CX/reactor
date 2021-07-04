//
// Created by suncx on 2020/8/8.
//

#include "Timestamp.h"
#include "GnuExt.h"
#include "ConsoleStream.h"

using std::string;
using reactor::core::Timestamp;

const int Timestamp::factors[] = {1, 1000, 1000 * 1000};

Timestamp::Timestamp(int64_t time_since_epoch, TimeUint unit) :
        microseconds_since_epoch(time_since_epoch * factors[unit]) {}

bool Timestamp::valid() const {
    return microseconds_since_epoch > 0;
}

Timestamp &Timestamp::operator+(const Timestamp &timestamp) {
    microseconds_since_epoch += timestamp.microseconds_since_epoch;
    return *this;
}

Timestamp Timestamp::operator+(const Timestamp &timestamp) const {
    return Timestamp(microseconds_since_epoch + timestamp.microseconds_since_epoch);
}

Timestamp &Timestamp::operator-(const Timestamp &timestamp) {
    microseconds_since_epoch -= timestamp.microseconds_since_epoch;
    return *this;
}

Timestamp Timestamp::operator-(const Timestamp &timestamp) const {
    return Timestamp(microseconds_since_epoch - timestamp.microseconds_since_epoch);
}

bool Timestamp::operator<(const Timestamp &rhs) const {
    return microseconds_since_epoch < rhs.microseconds_since_epoch;
}

bool Timestamp::operator>(const Timestamp &rhs) const {
    return rhs < *this;
}

bool Timestamp::operator<=(const Timestamp &rhs) const {
    return !(rhs < *this);
}

bool Timestamp::operator>=(const Timestamp &rhs) const {
    return !(*this < rhs);
}

bool Timestamp::operator==(const Timestamp &rhs) const {
    return microseconds_since_epoch == rhs.microseconds_since_epoch;
}

bool Timestamp::operator!=(const Timestamp &rhs) const {
    return !(rhs == *this);
}

Timestamp Timestamp::now() {
    timespec ts;
    auto status = clock_gettime(CLOCK_REALTIME, &ts);
    if (unlikely(status != 0)) RC_FATAL << "get clock time error!";
    return Timestamp(ts.tv_sec * factors[SECOND] + ts.tv_nsec / 1000);
}

string Timestamp::to_string(bool show_microsecond) const {
    char buf[32];
    time_t seconds = microseconds_since_epoch / factors[SECOND];
    tm tm_time;
    localtime_r(&seconds, &tm_time);

    if (show_microsecond) {
        auto ms = microseconds_since_epoch % factors[SECOND];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
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
    return {microseconds_since_epoch / factors[SECOND], microseconds_since_epoch % factors[SECOND] * 1000};
}

Timestamp reactor::core::operator ""_s(unsigned long long x) {
    return Timestamp(x, Timestamp::TimeUint::SECOND);
}

Timestamp reactor::core::operator ""_ms(unsigned long long x) {
    return Timestamp(x, Timestamp::TimeUint::MILLISECOND);
}

Timestamp reactor::core::operator ""_us(unsigned long long x) {
    return Timestamp(x, Timestamp::TimeUint::MICROSECOND);
}