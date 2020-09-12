#include <sys/timerfd.h>
#include <unistd.h>
#include <poll.h>
#include <cstdio>
#include <cstring>
#include <cstdint>

void test1() {
    auto timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);


    itimerspec it;
    memset(&it, 0, sizeof(it));
//    it.it_value.tv_sec = 2;
//    it.it_interval.tv_sec = 2;
//    it.it_interval.tv_nsec = 0;

    pollfd pfd;
    pfd.fd = timer_fd;
    pfd.events = POLLIN | POLLPRI;
    pfd.revents = 0;

    /**
     * flags：0 代表使用相对时间；TFD_TIMER_ABSTIME 使用绝对时间
     */
    timerfd_settime(timer_fd, 0, &it, nullptr);
//
//    itimerspec it2 = it;
//    it2.it_value.tv_sec = 1;
//    timerfd_settime(timer_fd, 0, &it2, nullptr);


    uint64_t val;
    for (int i = 0; i < 10; ++i) {
        auto num_ev = poll(&pfd, 1, -1);
        if (num_ev > 0) {
            if (pfd.revents & (POLLIN | POLLPRI | POLLRDHUP)) {
                printf("alarm...\n");
                read(timer_fd, &val, sizeof(val));
            }
        }
    }
    close(timer_fd);
}

int main(int argc, const char *argv[]) {
//    auto start = std::chrono::system_clock::now();
//
//    printf("now: %ld\n", start.time_since_epoch().count());
//
//    sleep(30);
//    timeval val;
//    gettimeofday(&val, nullptr);
//    printf("now: %ld\n", val.tv_sec * 1000000 + val.tv_usec);
//    printf("now: %ld\n", Timestamp::now().time_since_epoch());

    test1();

//    auto now = Timestamp::now();
//    printf("seconds: %ld, nsec: %ld\n", now.time_since_epoch() / (1000 * 1000),
//           now.time_since_epoch() % (1000 * 1000) * 1000);
//
//    auto s = now.to_timespec();
//    printf("sec: %ld, nsec: %ld\n", s.tv_sec, s.tv_nsec);
    return 0;
}