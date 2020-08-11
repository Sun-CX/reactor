//
// Created by suncx on 2020/8/11.
//

#ifndef REACTOR_CIRCULARBUFFER_H
#define REACTOR_CIRCULARBUFFER_H

#include <cstddef>
#include <iostream>
#include "Exception.h"

using std::ostream;
using std::endl;
using std::cout;

template<class T>
class CircularBuffer {
private:
    size_t capacity;
    int front, rear;
    T *arr;
public:
    explicit CircularBuffer(size_t max_size) : capacity(max_size + 1), front(0), rear(0) {
        arr = new T[capacity];
    }

    virtual ~CircularBuffer() {
        delete[] arr;
    }

    void push_back(const T &x) {
//        if (full()) ERROR_EXIT("full.");

        arr[rear] = x;
        if (full()) front = (front + 1) % capacity;
        rear = (rear + 1) % capacity;
    }

    T pop_front() {
        if (empty()) ERROR_EXIT("empty.");
        T val(move(arr[front]));
        front = (front + 1) % capacity;
        return val;
    }

    bool empty() const {
        return front == rear;
    }

    bool full() const {
        return (rear + 1) % capacity == front;
    }

    size_t size() const {
        return (rear + capacity - front) % capacity;
    }

    friend ostream &operator<<(ostream &os, const CircularBuffer &buffer) {
        os << "capacity: " << buffer.capacity << endl;
        for (int i = buffer.front; i != buffer.rear; i = (i + 1) % buffer.capacity) {
            os << buffer.arr[i] << ' ';
        }
        os << endl;
        return os;
    }
};

#endif //REACTOR_CIRCULARBUFFER_H
