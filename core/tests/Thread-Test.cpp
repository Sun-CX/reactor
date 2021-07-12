//
// Created by suncx on 2020/8/8.
//

#include "Thread.h"
#include "ConsoleStream.h"
#include <atomic>

using std::bind;
using reactor::core::CurrentThread;
using reactor::core::Thread;
using std::atomic_int;

void test_thread_longest_name() {

    auto func = [] {};

    Thread th(func);
    RC_DEBUG << th.get_name();

    Thread th2(func, "child-thread");
    RC_DEBUG << th2.get_name();

    // 16 bytes long
    Thread th3(func, "123456789012345678");
    RC_DEBUG << th3.get_name();
}

class Foo {
private:
    atomic_int count;
public:
    explicit Foo(int count = 0) : count(count) {}

    void increment(int n) {
        for (int i = 0; i < n; ++i)
            count++;

        RC_DEBUG << "CurrentThread name: " << CurrentThread::name << ", CurrentThread id: " << CurrentThread::id
                 << ", is main thread: " << CurrentThread::is_main_thread();
    }

    [[nodiscard]]
    int get_count() const {
        return count;
    }
};

void multi_thread_example() {

    RC_DEBUG << "CurrentThread name: " << CurrentThread::name << ", CurrentThread id: " << CurrentThread::id
             << ", is main thread: " << CurrentThread::is_main_thread();

    Foo foo;

    Thread th1(bind(&Foo::increment, &foo, 1000000));
    Thread th2(bind(&Foo::increment, &foo, 1000000));

    th1.start();
    th2.start();

    RC_DEBUG << "th1 thread name: " << th1.get_name() << ", id: " << th1.get_id();
    RC_DEBUG << "th2 thread name: " << th2.get_name() << ", id: " << th2.get_id();

    th1.join();
    th2.join();

    RC_DEBUG << "count: " << foo.get_count();
}

int main(int argc, const char *argv[]) {

    // test_thread_longest_name();
    multi_thread_example();

    return 0;
}