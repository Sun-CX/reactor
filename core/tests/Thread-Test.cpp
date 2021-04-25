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
        printf("CurrentThread name: %s, id: %d, is main thread: %d\n", CurrentThread::name,
               CurrentThread::id, CurrentThread::is_main_thread());
    }

    int get_count() const {
        return count;
    }
};

int main(int argc, const char *argv[]) {

//    CurrentThread::sleep(1000 * 5);

    printf("main thread name: %s, main thread id: %d, is main thread: %d\n", CurrentThread::name,
           CurrentThread::id,
           CurrentThread::is_main_thread());

    Foo foo;

    Thread t1(bind(&Foo::increment, &foo, 1000000));
    Thread t2(bind(&Foo::increment, &foo, 1000000));

    t1.start();
    t2.start();

    printf("t1 thread name: %s, id: %d\n", t1.name().c_str(), t1.getid());
    printf("t2 thread name: %s, id: %d\n", t2.name().c_str(), t2.getid());

    t1.join();
    t2.join();

    printf("count: %d\n", foo.get_count());

    return 0;
}