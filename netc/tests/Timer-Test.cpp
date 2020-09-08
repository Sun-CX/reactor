//
// Created by suncx on 2020/9/3.
//

#include "TimerTask.h"
#include "TimerId.h"
#include "CurrentThread.h"
#include "EventLoop.h"
#include <vector>
#include <set>
#include <algorithm>

using std::pair;
using std::vector;
using std::set;
using std::for_each;
using std::bind;

using Entry = pair<Timestamp, TimerTask *>;
using Timers = set<Entry>;

void test() {
    auto timer1 = new TimerTask([] {
        printf("timer1\n");
    }, Timestamp(1000), 0);

    auto timer2 = new TimerTask([] {
        printf("timer2\n");
    }, Timestamp(500), 0);

    auto timer3 = new TimerTask([] {
        printf("timer3\n");
    }, Timestamp(300), 0);

    auto timer4 = new TimerTask([] {
        printf("timer4\n");
    }, Timestamp(100), 0);

    Timers timers;
    printf("empty? %d\n", timers.empty());
    printf("empty? %d\n", timers.begin() == timers.end());
    timers.insert({timer1->expire_time(), timer1});

    printf("empty? %d\n", timers.empty());
    printf("empty? %d\n", timers.begin() == timers.end());
    timers.insert({timer2->expire_time(), timer2});
    timers.insert({timer3->expire_time(), timer3});
    timers.insert({timer4->expire_time(), timer4});

    for (auto const &e:timers) {
        printf("expire time: %ld, addr: %p\n", e.first.microseconds_since_epoch(), e.second);
    }

    delete timer4;
    delete timer3;
    delete timer2;
    delete timer1;
}

void test2() {
    set<int> ss;
    ss.insert(5);
    ss.insert(8);
    ss.insert(1);
    ss.insert(3);
    ss.insert(9);


    for (auto it = ss.cbegin(); it != ss.cend(); it++) {
        if (*it == 8) ss.erase(it);
    }

    for_each(ss.cbegin(), ss.cend(), [](int v) {
        printf("%d ", v);
    });

//    auto end = ss.lower_bound(7);
//    printf("end: %d\n", *end);
//    for_each(ss.begin(), end, [](int v) {
//        printf("%d ", v);
//    });
//
//    printf("\n");
//    vector<int> vec;
//    copy(ss.cbegin(), end, back_inserter(vec));
//    for (auto e:vec) printf("%d ", e);
}

static int count = 0;
static EventLoop *g_loop;

static void print_pid() {
    printf("now: %s, pid: %d, tid: %d\n", Timestamp::now().to_fmt_string().c_str(), getpid(), CurrentThread::pid);
}

static void print(const char *str) {
    printf("msg: %s, now: %s\n", str, Timestamp::now().to_fmt_string().c_str());
    if (++count == 20) g_loop->quit();
}

static void cancel(TimerId id) {
    g_loop->cancel(id);
    printf("cancel at: %s\n", Timestamp::now().to_fmt_string().c_str());
}

int main(int argc, const char *argv[]) {

    print_pid();

    EventLoop loop;
    g_loop = &loop;

    print("main");
    loop.run_after(bind(print, "once1"), 1);
    loop.run_after(bind(print, "once1.5"), 1.5);
    loop.run_after(bind(print, "once2.5"), 2.5);
    loop.run_after(bind(print, "once3.5"), 3.5);

    auto t = loop.run_after(bind(print, "once4.5"), 4.5);
    loop.run_after(bind(cancel, t), 4.2);
    loop.run_after(bind(cancel, t), 4.8);

    loop.run_every(bind(print, "every2"), 2);

    auto t2 = loop.run_every(bind(print, "every3"), 3);
    loop.run_after(bind(cancel, t2), 9.001);

    loop.loop();
    return 0;
}