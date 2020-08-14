//
// Created by suncx on 2020/8/9.
//

#include "../Mutex.h"
#include "../Timestamp.h"
#include "../Thread.h"
#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

static Mutex mutex;

vector<int> vec;

const int COUNT = 10 * 1000 * 1000;

void thread_func() {
    for (int i = 0; i < COUNT; ++i) {
        MutexGuard guard(mutex);
        vec.push_back(i);
    }
}

int count = 0;

int foo() {
    MutexGuard guard(mutex);
    if (!mutex.is_locked_by_cur_thread()) {
        printf("Fail\n");
        return -1;
    }
    ++count;
    return 0;
}

int main(int argc, const char *argv[]) {
    foo();

    if (count != 1) {
        printf("foo() called twice.\n");
        abort();
    }

    const int max_threads = 8;
    vec.reserve(max_threads * COUNT);

    Timestamp start(Timestamp::now());
    for (int i = 0; i < COUNT; ++i) {
        vec.push_back(i);
    }

    printf("single thread time used: %f\n", time_diff(Timestamp::now(), start));

    for (int n_threads = 1; n_threads < max_threads; ++n_threads) {
        vector<unique_ptr<Thread>> threads;
        vec.clear();

        start = Timestamp::now();
        threads.reserve(n_threads);

        for (int i = 0; i < n_threads; ++i) {
            threads.emplace_back(new Thread(thread_func));
            threads.back()->start();
        }

        for (int i = 0; i < n_threads; ++i) {
            threads[i]->join();
        }
        printf("%d thread(s) with lock time used: %f\n", n_threads, time_diff(Timestamp::now(), start));
    }

    return 0;
}
