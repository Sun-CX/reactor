//
// Created by suncx on 2020/8/10.
//
#include "../BlockingQueue.h"

int main(int argc, const char *argv[]) {

    BlockingQueue<int> bq;

    printf("bq default size: %zu\n", bq.size());

    return 0;
}