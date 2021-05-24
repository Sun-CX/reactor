//
// Created by ysm on 8/10/20.
//

#include <iostream>
#include <memory>
#include "Singleton.h"
#include "Thread.h"

using std::shared_ptr;
using std::cout;
using std::endl;

class Student {
private:
    string name;
};

void get_instance() {
    shared_ptr<Student> student = Singleton<Student>::instance();
    cout << student.get() << endl;
}

int main(int argc, const char *argv[]) {

    printf("equal? %d\n", make_singleton<Student>() == make_singleton<Student>());

    shared_ptr<Student> student = Singleton<Student>::instance();
    cout << student.get() << endl;

    Thread another_thread([] { return get_instance(); });
    another_thread.start();
    another_thread.join();
    return 0;
}
