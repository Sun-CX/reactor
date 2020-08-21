//
// Created by suncx on 2020/8/8.
//

#include "Thread.h"
#include "CurrentThread.h"

using std::bind;

class Foo {
private:
    int count;
public:
    explicit Foo(int count = 0) : count(count) {}

    void increment(int n) {
        for (int i = 0; i < n; ++i) {
            count++;
        }
        printf("CurrentThread name: %s, pid: %d\n", CurrentThread::name, CurrentThread::pid);
    }

    int get_count() const {
        return count;
    }
};

int main(int argc, const char *argv[]) {
    printf("main thread name: %s, main thread pid: %d\n", CurrentThread::name, CurrentThread::pid);

    Foo foo;

    Thread t1(bind(&Foo::increment, &foo, 1000000));
    Thread t2(bind(&Foo::increment, &foo, 1000000));

    t1.start();
    t2.start();

    t1.join();
    t2.join();

    printf("t1 name: %s, pid: %d\n", t1.get_name().c_str(), t1.get_tid());
    printf("t2 name: %s, pid: %d\n", t2.get_name().c_str(), t2.get_tid());

    printf("count: %d\n", foo.get_count());

    return 0;
}