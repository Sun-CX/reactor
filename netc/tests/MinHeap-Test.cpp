//
// Created by suncx on 2020/9/7.
//

#include "MinHeap.h"
#include <cstdio>

int main(int argc, const char *argv[]) {

    MinHeap<int> heap;
    heap.insert(6);
    heap.insert(2);
    heap.insert(-2);
    heap.insert(86);
    heap.insert(96);
//
//    printf("current size: %ld\n", heap.size());


//    heap.pop();
//    heap.print_all();
//    heap.pop();
//    heap.print_all();
//    heap.pop();
//    heap.print_all();
//
//    heap.pop();
//    heap.print_all();
//
//    heap.pop();
//    heap.print_all();
    int val;

    while (not heap.empty()) {
        val = heap.pop();
        printf("%d ", val);
    }

    heap.pop();

    return 0;
}