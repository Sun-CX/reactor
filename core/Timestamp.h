//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_TIMESTAMP_H
#define REACTOR_TIMESTAMP_H

#include <string>

namespace reactor::core {
    using std::string;

    class Timestamp final {
    private:
        int64_t microseconds_since_epoch;     // since 1970-01-01 00:00:00 +0000 (UTC).

        explicit Timestamp(int64_t microseconds);

    public:

        Timestamp();

        [[nodiscard]]
        string to_string(bool show_ms = false) const;

        [[nodiscard]]
        timespec to_timespec() const;

        [[nodiscard]]
        int64_t time_since_epoch() const;

        Timestamp &operator+(const Timestamp &ts);

        Timestamp operator+(const Timestamp &ts) const;

        Timestamp &operator-(const Timestamp &ts);

        Timestamp operator-(const Timestamp &ts) const;

        bool operator<(const Timestamp &ts) const;

        bool operator>(const Timestamp &ts) const;

        bool operator<=(const Timestamp &ts) const;

        bool operator>=(const Timestamp &ts) const;

        bool operator==(const Timestamp &ts) const;

        bool operator!=(const Timestamp &ts) const;

        static Timestamp now();
    };
}

#endif //REACTOR_TIMESTAMP_H
