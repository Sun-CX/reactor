//
// Created by suncx on 2020/8/9.
//

#include "CountDownLatch.h"
#include "ConsoleStream.h"

CountDownLatch::CountDownLatch(int count) : mutex(), cond(mutex) {
    if (count < 0) FATAL << "illegal argument count: " << count;
    this->count = count;
}

void CountDownLatch::wait() {
    MutexGuard guard(mutex);
    while (count > 0) {
        cond.wait();
    }
}

void CountDownLatch::count_down() {
    MutexGuard guard(mutex);
    count--;
    if (count == 0) cond.notify_all();
}

int CountDownLatch::get_count() const {
    MutexGuard guard(mutex);
    return count;
}
