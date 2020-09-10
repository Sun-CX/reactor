//
// Created by suncx on 2020/8/8.
//
#include "Timestamp.h"
#include <vector>

using std::vector;

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

//    printf("time: %s\n", Timestamp::now().to_string().c_str());
//
//    Timestamp t1;
//    Timestamp t2(2000000);
//
//    printf("%d\n", t1 < t2);
//    printf("diff: %f\n", time_diff(t2, t1));
//
//    printf("%s\n", Timestamp::now().to_fmt_string().c_str());
//    printf("%s\n", Timestamp::now().to_fmt_string(true).c_str());

//    benchmark();

    Timestamp s = 1000_ms;
    printf("%s\n", s.to_string().c_str());

    return 0;
}