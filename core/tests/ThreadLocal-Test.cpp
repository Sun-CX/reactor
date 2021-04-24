//
// Created by suncx on 2020/8/15.
//

#include "ThreadLocal.h"
#include "CurrentThread.h"
#include "Thread.h"

class Test {
private:
    string name;
public:
    Test() {
        printf("%s[%d] constructing %p\n", CurrentThread::name, CurrentThread::pid, this);
    }

    virtual ~Test() {
        printf("%s[%d] destructing %p, thread_name: %s\n", CurrentThread::name, CurrentThread::pid, this, name.c_str());
    }

    [[nodiscard]]
    const string &getName() const {
        return name;
    }

    void setName(const string &n) {
        this->name = n;
    }
};

ThreadLocal<Test> local1;
ThreadLocal<Test> local2;

static void print() {
    printf("%s[%d] obj1 %p, thread_name: %s\n", CurrentThread::name, CurrentThread::pid, &local1.get_value(),
           local1.get_value().getName().c_str());

    printf("%s[%d] obj2 %p, thread_name: %s\n", CurrentThread::name, CurrentThread::pid, &local2.get_value(),
           local2.get_value().getName().c_str());
}

static void thread_routine() {
    print();
    local1.get_value().setName("one");
    local2.get_value().setName("two");
    print();
}

int main(int argc, const char *argv[]) {
    local1.get_value().setName("main one");
    print();

    Thread t1(thread_routine);
    t1.start();
    t1.join();

    local2.get_value().setName("main two");
    print();

//    pthread_exit(nullptr);
    return 0;
}