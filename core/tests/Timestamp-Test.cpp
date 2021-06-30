//
// Created by suncx on 2020/8/8.
//
#include "Timestamp.h"
#include <vector>

using std::vector;
using reactor::core::Timestamp;

void benchmark() {
    const int n = 1000 * 1000;

    vector<Timestamp> vec;
    vec.reserve(n);
    for (int i = 0; i < n; ++i) {
        vec.push_back(Timestamp::now());
    }
    printf("begin: %s\n", vec.front().to_string().c_str());
    printf("end: %s\n", vec.back().to_string().c_str());
    printf("insert %d timestamps time used: %ld microseconds.\n", n, (vec.back() - vec.front()).time_since_epoch());
}

int main(int argc, const char *argv[]) {
    benchmark();
    return 0;
}