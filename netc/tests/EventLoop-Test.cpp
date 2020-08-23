//
// Created by suncx on 2020/8/18.
//

#include "CurrentThread.h"
#include "EventLoop.h"
#include "Channel.h"
#include <cstdio>
#include <sys/timerfd.h>
#include <cstring>

using std::bind;

static EventLoop *g_loop;

static int count = 1;

void time_out(int fd) {
    if (count == 10) {
        g_loop->quit();
    }
    char buf[1024];
    auto n = read(fd, buf, sizeof(buf));
    printf("has read: %ld, event triggered for %d times.\n", n, count);
    count++;
}

int main(int argc, const char *argv[]) {

    EventLoop loop;
    g_loop = &loop;

    auto timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    Channel channel(g_loop, timerfd);
    channel.set_read_callback(bind(time_out, timerfd));
    channel.enable_reading();

    itimerspec spec;

    memset(&spec, 0, sizeof(spec));
    spec.it_value.tv_sec = 3;
    spec.it_interval.tv_sec = 1;

    timerfd_settime(timerfd, 0, &spec, nullptr);

    g_loop->loop();
    close(timerfd);
    return 0;
}