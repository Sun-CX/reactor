//
// Created by suncx on 2020/8/18.
//

#include "CurrentThread.h"
#include "EventLoop.h"
#include "Channel.h"
#include <cstdio>
#include <sys/timerfd.h>
#include <cstring>

static EventLoop *g_loop;

void time_out() {
    printf("timeout...\n");
    g_loop->quit();
}

int main(int argc, const char *argv[]) {

    EventLoop loop;
    g_loop = &loop;

    auto timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    Channel channel(g_loop, timerfd);
    channel.set_read_callback(time_out);
    channel.enable_reading();

    itimerspec spec;

    memset(&spec, 0, sizeof(spec));
    spec.it_value.tv_sec = 4;

    timerfd_settime(timerfd, 0, &spec, nullptr);

    g_loop->loop();
    close(timerfd);
    return 0;
}