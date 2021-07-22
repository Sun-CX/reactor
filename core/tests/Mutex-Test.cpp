//
// Created by suncx on 2020/8/9.
//

#include "Mutex.h"
#include "Thread.h"
#include <vector>
#include <memory>
#include "ConsoleStream.h"

using std::vector;
using std::unique_ptr;
using std::chrono::system_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;
using reactor::core::Mutex;
using reactor::core::MutexGuard;
using reactor::core::Thread;

static Mutex mutex;

static int count = 0;

vector<int> vec;

const int COUNT = 10 * 1000 * 1000;

void thread_func() {
    for (int i = 0; i < COUNT; ++i) {
        MutexGuard guard(mutex);
        vec.push_back(i);
    }
}

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

    system_clock::time_point start = system_clock::now();

    for (int i = 0; i < COUNT; ++i) {
        vec.push_back(i);
    }

    system_clock::time_point end = system_clock::now();

    RC_DEBUG << "single thread time used: " << duration_cast<microseconds>(end - start).count() << " us.";

    for (int n_threads = 1; n_threads <= max_threads; ++n_threads) {
        vector<unique_ptr<Thread>> threads;
        vec.clear();

        threads.reserve(n_threads);
        start = system_clock::now();

        for (int i = 0; i < n_threads; ++i) {
            threads.emplace_back(new Thread(thread_func));
            threads.back()->start();
        }

        for (int i = 0; i < n_threads; ++i) {
            threads[i]->join();
        }

        end = system_clock::now();

        RC_DEBUG << n_threads << " thread(s) with mutex lock time used: "
                 << duration_cast<microseconds>(end - start).count() << " us.";
    }

    return 0;
}
