//
// Created by ysm on 8/10/20.
//

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <functional>
#include "../Singleton.h"

class Student
{
public:
    Student() = default;
    ~Student() = default;
private:
    std::string name;
};

void get_instance()
{
    std::shared_ptr<Student> student = Singleton<Student>::instance();
    std::cout << student.get() << std::endl;
}

int main(int argc, char *argv[])
{
    std::shared_ptr<Student> student = Singleton<Student>::instance();
    std::cout << student.get() << std::endl;
    std::cout << student.get() << std::endl;

    std::thread another_thread(std::bind(get_instance));

    another_thread.join();

    return 0;
}
