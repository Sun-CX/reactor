//
// Created by 孙诚雄 on 2021/7/1.
//

#include "URLEncoder.h"
#include "ConsoleStream.h"

using reactor::net::URLEncoder;

int main(int argc, const char *argv[]) {

    RC_DEBUG << URLEncoder::encode("/hello world?name=sd ds&val=sd");

    return 0;
}