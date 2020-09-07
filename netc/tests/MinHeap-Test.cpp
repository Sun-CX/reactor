//
// Created by suncx on 2020/9/7.
//

#include "MinHeap.h"
#include "Timestamp.h"
#include <cstdio>

class TimerTask {
private:
    Timestamp expired_time;
public:
    explicit TimerTask(const Timestamp &expiredTime) : expired_time(expiredTime) {}

    bool operator<(const TimerTask &rhs) const {
        return expired_time < rhs.expired_time;
    }

    void print_expired_time() const {
        printf("%s\n", expired_time.to_fmt_string(true).c_str());
    }
};

int main(int argc, const char *argv[]) {

    MinHeap<int, 4> heap;
    heap.insert(6);
    heap.insert(2);
    heap.insert(-2);
    heap.insert(86);
    heap.insert(96);

//    printf("current size: %ld\n", heap.size());


//    heap.pop();
//    heap.print_all();
//    heap.pop();
//    heap.print_all();
//    heap.pop();
//    heap.print_all();
//
//    heap.pop();
//    heap.print_all();
//
//    heap.pop();
//    heap.print_all();
    int val;

    while (not heap.empty()) {
        val = heap.pop();
        printf("%d ", val);
    }
    printf("\n");

//    heap.pop();

    MinHeap<TimerTask, 4> h;
    h.insert(TimerTask(Timestamp(6)));
    h.insert(TimerTask(Timestamp(15)));
    h.insert(TimerTask(Timestamp(2)));
    h.insert(TimerTask(Timestamp(15)));
    h.insert(TimerTask(Timestamp(52)));

    h.pop().print_expired_time();
    h.pop().print_expired_time();
    h.pop().print_expired_time();
    h.pop().print_expired_time();
    h.pop().print_expired_time();

    return 0;
}