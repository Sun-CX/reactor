//
// Created by suncx on 2020/9/7.
//

#include "MinHeap.h"
#include <cstdio>

int main(int argc, const char *argv[]) {

    QuadHeap<int *> heap;
    heap.insert(new int(5));
    heap.insert(new int(-5));
    heap.insert(new int(12));
    heap.insert(new int(5));
    heap.insert(new int(56));

    int *p;
    while (not heap.empty()) {
        p = heap.pop();
        printf("%d ", *p);
        delete p;
    }
    return 0;
}