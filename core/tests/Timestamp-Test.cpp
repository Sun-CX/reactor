//
// Created by suncx on 2020/8/8.
//
#include "../Timestamp.h"
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
    printf("insert %d timestamps time used: %f s.\n", n, time_diff(vec.back(), vec.front()));
}

int main(int argc, const char *argv[]) {

//    Timestamp time(1000 * 1000 * 30);

//    printf("time: %s\n", time.to_string().c_str());
//
//    Timestamp t1;
//    Timestamp t2(2000000);
//
//    printf("%d\n", t1 < t2);
//    printf("diff: %f\n", time_diff(t2, t1));
//
//    printf("%s\n", Timestamp::now().to_fmt_string().c_str());
//    printf("%s\n", Timestamp::now().to_fmt_string(true).c_str());

    benchmark();
    return 0;
}

