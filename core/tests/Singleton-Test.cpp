//
// Created by ysm on 8/10/20.
//

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include "Singleton.h"

using std::shared_ptr;

class Student {
private:
    std::string name;
};

void get_instance() {
    std::shared_ptr<Student> student = Singleton<Student>::instance();
    std::cout << student.get() << std::endl;
}

int main(int argc, const char *argv[]) {

    printf("equal? %d\n", make_singleton<Student>() == make_singleton<Student>());

    shared_ptr<Student> student = Singleton<Student>::instance();
    std::cout << student.get() << std::endl;

    std::thread another_thread([] { return get_instance(); });

    another_thread.join();
    return 0;
}
