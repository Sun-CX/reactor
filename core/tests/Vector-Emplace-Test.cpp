//
// Created by suncx on 2020/8/14.
//

#include <vector>
#include <cstdio>
#include <memory>

using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::shared_ptr;
using std::make_shared;

class Person final {
private:
    int age;
public:
    explicit Person(int age) : age(age) {
        printf("constructor invoked.\n");
    }

    virtual ~Person() {
        printf("destructor invoked.\n");
    }

    Person(const Person &person) : age(person.age) {
        printf("copy constructor invoked.\n");
    }

    Person(Person &&person) noexcept: age(person.age) {
        printf("move constructor invoked.\n");
    }

    Person &operator=(const Person &person) {
        age = person.age;
        printf("copy operator= invoked.\n");
        return *this;
    }

    Person &operator=(Person &&person) noexcept {
        age = person.age;
        printf("move operator= invoked.\n");
        return *this;
    }
};

void test1() {
    vector<Person> people;
    people.reserve(10);

    Person p1(18);      // 构造
    people.push_back(p1);   // 拷贝

    printf("--------------------------------------------\n");

    people.push_back(Person(20));   // 构造 + 移动
    printf("--------------------------------------------\n");

    people.emplace_back(24);    // 构造
    printf("--------------------------------------------\n");
    people.emplace_back(48);
}

void test2() {
    vector<Person *> people;
    people.push_back(new Person(12));   //内存泄漏
}

void test3() {
    vector<unique_ptr<Person>> people;
//    people.push_back(make_unique<Person>(16));
    people.emplace_back(new Person(24));
}

void test4() {
    vector<shared_ptr<Person>> people;
//    people.push_back(make_shared<Person>(18));
    people.emplace_back(new Person(25));
}

int main(int argc, const char *argv[]) {

//    test1();
//    test2();
//    test3();
//    test4();

    vector<int> vec(10);
    printf("size: %zu, capacity: %zu\n", vec.size(), vec.capacity());

    return 0;
}
