//
// Created by suncx on 2020/8/8.
//

#include "Thread.h"

using std::bind;

class Foo {
private:
    int count;
public:
    explicit Foo(int count = 0) : count(count) {}

    void increment(int n) {
        for (int i = 0; i < n; ++i) count++;
        printf("CurrentThread thread_name: %s, tid: %d\n", CurrentThread::name, CurrentThread::id);
    }

    int get_count() const {
        return count;
    }
};

int main(int argc, const char *argv[]) {
    printf("main thread thread_name: %s, main thread pid: %d\n", CurrentThread::name, CurrentThread::id);

    Foo foo;

    Thread t1(bind(&Foo::increment, &foo, 1000000));
    Thread t2(bind(&Foo::increment, &foo, 1000000));

    t1.start();
    t2.start();

    t1.join();
    t2.join();

    printf("t1 thread_name: %s, tid: %d\n", t1.name().c_str(), t1.getid());
    printf("t2 thread_name: %s, tid: %d\n", t2.name().c_str(), t2.getid());

    printf("count: %d\n", foo.get_count());

    return 0;
}