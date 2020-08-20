//
// Created by suncx on 2020/8/19.
//


#include <Buffer.h>

int main(int argc, const char *argv[]) {

    vector<int> vec(5);

    printf("size: %zu, capacity: %zu\n", vec.size(), vec.capacity());

    for (auto e:vec) printf("%d ", e);
    printf("\n");

    vec.resize(10);
    printf("size: %zu, capacity: %zu\n", vec.size(), vec.capacity());
    for (auto e:vec) printf("%d ", e);
    return 0;
}