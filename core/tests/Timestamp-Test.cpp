//
// Created by suncx on 2020/8/8.
//
#include "Timestamp.h"
#include "ConsoleStream.h"
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

    RC_DEBUG << "begin: " << vec.front().to_string(true);
    RC_DEBUG << "begin: " << vec.back().to_string(true);
    RC_DEBUG << "insert " << n << " timestamp use " << (vec.back() - vec.front()).time_since_epoch() << " us.";
}

int main(int argc, const char *argv[]) {
    benchmark();

    // RC_DEBUG << timestamp::now().to_string(true);

    return 0;
}