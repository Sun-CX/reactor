//
// Created by suncx on 2020/8/18.
//

#include "EventLoop.h"
#include "Channel.h"
#include "ConsoleStream.h"
#include <sys/timerfd.h>
#include <cstring>
#include <unistd.h>
#include <cassert>

using std::bind;
using reactor::net::EventLoop;
using reactor::net::Channel;

static EventLoop *g_loop;

static int count = 1;

void timeout(int fd) {

    uint64_t val;
    ssize_t n = ::read(fd, &val, sizeof(val));
    assert(n == sizeof(val));

    if (count == 10) {
        g_loop->quit();
    }
    DEBUG << "event triggered " << count << " times.";
    count++;
}

int main(int argc, const char *argv[]) {

    EventLoop loop;
    g_loop = &loop;

    auto timer_fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    Channel channel(g_loop, timer_fd);
    channel.set_read_callback([=] { return timeout(timer_fd); });
    channel.enable_reading();

    itimerspec spec;

    memset(&spec, 0, sizeof(spec));
    spec.it_value.tv_sec = 3;
    spec.it_interval.tv_sec = 1;

    timerfd_settime(timer_fd, 0, &spec, nullptr);

    loop.loop();
    ::close(timer_fd);
    return 0;
}