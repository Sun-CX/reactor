//
// Created by suncx on 2020/8/15.
//

#include "ThreadLocal.h"
#include "Thread.h"
#include "ConsoleStream.h"

using std::string;
using reactor::core::ThreadLocal;
using reactor::core::CurrentThread;
using reactor::core::Thread;

class Test {
private:
    string name;
public:
    Test() {
        RC_DEBUG << "constructed " << this;
    }

    ~Test() {
        RC_DEBUG << "destructed " << this;
    }

    [[nodiscard]]
    const string &get_name() const {
        return name;
    }

    void set_name(const string &n) {
        this->name = n;
    }
};

ThreadLocal<Test> local1;
ThreadLocal<Test> local2;

static void print() {
    RC_DEBUG << "obj1: " << &local1.get_value() << ", name: " << local1.get_value().get_name();

    RC_DEBUG << "obj2: " << &local2.get_value() << ", name: " << local2.get_value().get_name();
}

static void thread_routine() {
    print();
    local1.get_value().set_name("one");
    local2.get_value().set_name("two");
    print();
}

int main(int argc, const char *argv[]) {
    local1.get_value().set_name("main one");
    print();

    Thread t1(thread_routine);
    t1.start();
    t1.join();

    local2.get_value().set_name("main two");
    print();

    // pthread_exit(nullptr);
    return 0;
}