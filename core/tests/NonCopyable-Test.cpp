//
// Created by suncx on 2020/8/8.
//

#include "NonCopyable.h"

class Foo : public NonCopyable {

};

int main(int argc, const char *argv[]) {

    Foo f1;
    Foo f2(f1); // error

    Foo f3;
    f3 = f1;    // error

    return 0;
}
