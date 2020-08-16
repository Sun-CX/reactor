//
// Created by suncx on 2020/8/8.
//

#include "Exception.h"

class Fool {
public:
    void test() {
        throw Exception("oops");
    }
};

void foo() {
    Fool foo;
    foo.test();
}

int main(int argc, const char *argv[]) {

    try {
        foo();
    } catch (const Exception &ex) {
        printf("%s\n", ex.what());
        printf("%s\n", ex.stack_trace());
    } catch (...) {
        printf("catch all...");
    }

    return 0;
}

