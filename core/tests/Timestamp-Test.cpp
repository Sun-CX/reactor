//
// Created by 孙诚雄 on 2021/7/24.
//

#include "ConsoleStream.h"
#include "Timestamp.h"

using std::chrono::system_clock;
using reactor::core::to_string;
using reactor::core::to_utc_string;
using reactor::core::Timestamp;

int main(int argc, const char *argv[]) {

    RC_DEBUG << to_string(system_clock::now());
    RC_DEBUG << to_utc_string(system_clock::now());

    RC_DEBUG << sizeof(Timestamp);

    return 0;
}