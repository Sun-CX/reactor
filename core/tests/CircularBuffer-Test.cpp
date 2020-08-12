//
// Created by suncx on 2020/8/11.
//

#include "../CircularBuffer.h"

int main(int argc, const char *argv[]) {

    CircularBuffer<int> buffer(3);

    buffer.push_back(4);
    println(buffer);
    buffer.push_back(8);
    println(buffer);
    buffer.push_back(12);
    println(buffer);

    buffer.push_back(16);
    println(buffer);

    buffer.pop_front();
    println(buffer);
    buffer.pop_front();
    println(buffer);
    buffer.pop_front();
    println(buffer);

    buffer.pop_front();
    println(buffer);


    return 0;
}
