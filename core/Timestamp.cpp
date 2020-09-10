//
// Created by suncx on 2020/8/8.
//

#include "Timestamp.h"
#include "Exception.h"

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

Timestamp &Timestamp::operator-(const Timestamp &timestamp) {
    microseconds_since_epoch -= timestamp.microseconds_since_epoch;
    return *this;
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
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    return Timestamp(ts.tv_sec * factors[second] + ts.tv_nsec / 1000);
}

string Timestamp::to_string(bool show_microsecond) const {
    char buf[32];
    time_t seconds = microseconds_since_epoch / factors[second];
    tm tm_time;
    localtime_r(&seconds, &tm_time);

    if (show_microsecond) {
        auto ms = microseconds_since_epoch % factors[second];
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
    return {microseconds_since_epoch / factors[second], microseconds_since_epoch % factors[second] * 1000};
}