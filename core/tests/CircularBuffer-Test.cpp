//
// Created by suncx on 2020/8/11.
//

#include "../CircularBuffer.h"

int main(int argc, const char *argv[]) {

    CircularBuffer<int> buffer(3);

    buffer.push_back(4);
    cout << buffer;
    printf("current capacity: %zu\n", buffer.get_capacity());
    buffer.push_back(8);
    cout << buffer;
//    printf("current size: %zu\n", buffer.size());
    buffer.push_back(20);
    cout << buffer;
//    printf("current size: %zu\n", buffer.size());
    buffer.push_back(60);
    cout << buffer;
//    printf("current size: %zu\n", buffer.size());
    buffer.push_back(80);
    cout << buffer;
//    printf("current size: %zu\n", buffer.size());

//    int val = buffer.pop_front();
//    printf("pop val: %d, current size: %zu\n", val, buffer.size());

    return 0;
}
