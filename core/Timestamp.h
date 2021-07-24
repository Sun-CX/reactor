//
// Created by 孙诚雄 on 2021/7/24.
//

#ifndef REACTOR_TIMESTAMP_H
#define REACTOR_TIMESTAMP_H

#include <chrono>
#include <string>

namespace reactor::core {
    using std::string;
    using std::chrono::system_clock;
    using Timestamp = system_clock::time_point;

    // convert timestamp to local time string.
    string to_string(Timestamp ts);

    // convert timestamp to utc time string.
    string to_utc_string(Timestamp ts);
}

#endif //REACTOR_TIMESTAMP_H
