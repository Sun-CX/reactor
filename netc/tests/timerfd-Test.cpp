//#include <sys/timerfd.h>
//#include <ctime>
//#include <unistd.h>
//#include <cstdlib>
//#include <cstdio>
//#include <cstdint>        /* Definition of uint64_t */
//
//#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
//
//static void print_elapsed_time(void) {
//    static timespec start;
//    timespec curr;
//    static int first_call = 1;
//    int secs, nsecs;
//
//    if (first_call) {
//        first_call = 0;
//        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
//            handle_error("clock_gettime");
//    }
//
//    if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
//        handle_error("clock_gettime");
//
//    secs = curr.tv_sec - start.tv_sec;
//    nsecs = curr.tv_nsec - start.tv_nsec;
//    if (nsecs < 0) {
//        secs--;
//        nsecs += 1000000000;
//    }
//    printf("%d.%03d: ", secs, (nsecs + 500000) / 1000000);
//}
//
//int main(int argc, char *argv[]) {
//    itimerspec new_value;
//    int max_exp = 0, fd;
//    timespec now;
//    uint64_t exp, tot_exp;
//    ssize_t s;
//
//    if ((argc != 2) && (argc != 4)) {
//        fprintf(stderr, "%s init-secs [interval-secs max-exp]\n",
//                argv[0]);
//        exit(EXIT_FAILURE);
//    }
//
//    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
//        handle_error("clock_gettime");
//
//    /* Create a CLOCK_REALTIME absolute timer with initial
//       expiration and interval as specified in command line */
//
//    new_value.it_value.tv_sec = now.tv_sec + 3;
//    new_value.it_value.tv_nsec = now.tv_nsec;
////    if (argc == 2) {
////        new_value.it_interval.tv_sec = 0;
////        max_exp = 1;
////    } else {
////        new_value.it_interval.tv_sec = atoi(argv[2]);
////        max_exp = atoi(argv[3]);
////    }
//    new_value.it_interval.tv_sec = 1;
//    new_value.it_interval.tv_nsec = 0;
//
//    fd = timerfd_create(CLOCK_REALTIME, 0);
//    if (fd == -1)
//        handle_error("timerfd_create");
//
//    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
//        handle_error("timerfd_settime");
//
////    print_elapsed_time();
//    printf("timer started\n");
//
//    for (tot_exp = 0; tot_exp < 10; tot_exp++) {
//        s = read(fd, &exp, sizeof(uint64_t));
//        if (s != sizeof(uint64_t)) handle_error("read");
//
////        tot_exp += exp;
////        print_elapsed_time();
//        printf("read: %llu; total=%llu\n",
//               (unsigned long long) exp,
//               (unsigned long long) tot_exp);
//    }
//
//    exit(EXIT_SUCCESS);
//}

#include <sys/timerfd.h>
#include <unistd.h>
#include <poll.h>
#include <cstdio>
#include <cstdint>

int main(int argc, const char *argv[]) {
//    timespec n1, n2;
//    clock_gettime(CLOCK_REALTIME, &n1);
//    clock_gettime(CLOCK_MONOTONIC, &n2);
//    printf("[realtime] sec: %ld, ns: %ld\n[monotonic] sec: %ld, ns: %ld\n", n1.tv_sec, n1.tv_nsec, n2.tv_sec, n2.tv_nsec);


    auto timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
//
//    timespec now;
//    clock_gettime(CLOCK_MONOTONIC, &now);
//
    itimerspec it;
    it.it_value.tv_sec = 7;
    it.it_value.tv_nsec = 0;
    it.it_interval.tv_sec = 2;
    it.it_interval.tv_nsec = 0;

    pollfd pfd;
    pfd.fd = timer_fd;
    pfd.events = POLLIN | POLLPRI;
    pfd.revents = 0;

    /**
     * flags：0 代表使用相对时间；TFD_TIMER_ABSTIME 使用绝对时间
     */
    timerfd_settime(timer_fd, 0, &it, nullptr);


    uint64_t val;
    for (int i = 0; i < 10; ++i) {
        auto num_ev = poll(&pfd, 1, -1);
        if (num_ev > 0) {
            if (pfd.revents & (POLLIN | POLLPRI | POLLRDHUP)) {
                printf("alarm...\n");
                read(timer_fd, &val, sizeof(val));
//                printf("read val: %lu\n", val);
            }
        }
    }


//
//    uint64_t val;
//
//    for (int i = 0; i < 10; ++i) {
//        read(timer_fd, &val, sizeof(val));
//        printf("read val: %lu\n", val);
//    }
    close(timer_fd);
    return 0;
}