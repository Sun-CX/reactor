//
// Created by 孙诚雄 on 2021/7/23.
//

#include <sys/timerfd.h>
#include <unistd.h>
#include <cassert>
#include <chrono>
#include "ConsoleStream.h"

using std::chrono::steady_clock;
using std::chrono::seconds;
using std::chrono::duration_cast;
using std::chrono_literals::operator ""s;

int main(int argc, const char *argv[]) {

    int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);

    assert(fd > 0);

    itimerspec its;

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;


    steady_clock::time_point now = steady_clock::now();
    steady_clock::time_point point = now + 3s;

    its.it_value.tv_sec = duration_cast<seconds>(point.time_since_epoch()).count();
    its.it_value.tv_nsec = point.time_since_epoch().count() % 1000000000;

    int ret = ::timerfd_settime(fd, TFD_TIMER_ABSTIME, &its, nullptr);
    RC_DEBUG << "ret = " << ret;

    point = now + 5s;
    its.it_value.tv_sec = duration_cast<seconds>(point.time_since_epoch()).count();
    its.it_value.tv_nsec = point.time_since_epoch().count() % 1000000000;

    ret = ::timerfd_settime(fd, TFD_TIMER_ABSTIME, &its, nullptr);
    RC_DEBUG << "ret = " << ret;

    while (true) {
        uint64_t val;
        ssize_t n = ::read(fd, &val, sizeof(val));
        assert(n == sizeof(val));

        RC_DEBUG << "done.";
    }

    return 0;
}