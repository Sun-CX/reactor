//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_TIMESTAMP_H
#define REACTOR_TIMESTAMP_H

#include <string>

using std::string;

class Timestamp final {
private:
    int64_t microseconds_since_epoch;     // since 1970-01-01 00:00:00 +0000 (UTC).
    static const int factors[];
public:
    enum TimeUint {
        microsecond, millisecond, second
    };

    explicit Timestamp(int64_t time_since_epoch = 0, TimeUint unit = microsecond);

    [[nodiscard]]
    bool valid() const;

    [[nodiscard]]
    string to_string(bool show_microsecond = false) const;

    [[nodiscard]]
    timespec to_timespec() const;

    [[nodiscard]]
    int64_t time_since_epoch() const;

    Timestamp &operator+(const Timestamp &timestamp);

    Timestamp &operator-(const Timestamp &timestamp);

    bool operator<(const Timestamp &rhs) const;

    bool operator>(const Timestamp &rhs) const;

    bool operator<=(const Timestamp &rhs) const;

    bool operator>=(const Timestamp &rhs) const;

    bool operator==(const Timestamp &rhs) const;

    bool operator!=(const Timestamp &rhs) const;

    static Timestamp now();
};

#endif //REACTOR_TIMESTAMP_H
