//
// Created by suncx on 2020/8/14.
//

#include <cstdio>
#include "ConsoleStream.h"

#define RED "\e[0;31m"
#define NONE "\e[0m"
#define GREEN "\e[0;32m"
#define L_GREEN "\e[1;32m"
#define BRIGHT_YELLOW  "\e[1;33m"

int main(int argc, const char *argv[]) {


    LOG << false;
    INFO << true << "asjkdcasjkn";
    ERROR << true;

//    console.log() << false;

    return 0;
}