#include <sys/timerfd.h>
#include <unistd.h>
#include <poll.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <memory>

using std::shared_ptr;
using std::make_shared;

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

static void echo(const shared_ptr<int> &ptr) {
    printf("echo: use_count: %ld, value: %d\n", ptr.use_count(), *ptr);
}

int main(int argc, const char *argv[]) {
//    test1();

//    auto p = make_shared<int>(6);
    shared_ptr<int> p(new int(6));
    printf("main, use_count: %ld\n", p.use_count());
    echo(p);
    printf("main, use_count: %ld\n", p.use_count());
    return 0;
}