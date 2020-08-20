//
// Created by suncx on 2020/8/17.
//

#include <TimeZone.h>

int main(int argc, const char *argv[]) {

    auto dp = new Data();

    read_timezone_file("/etc/localtime", dp);

    
    

    return 0;
}