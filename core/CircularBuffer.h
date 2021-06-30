//
// Created by suncx on 2020/8/11.
//

#ifndef REACTOR_CIRCULARBUFFER_H
#define REACTOR_CIRCULARBUFFER_H

#include "ConsoleStream.h"
#include "GnuExt.h"

using std::move;

namespace reactor::core {
    /**
     * 循环队列，亦称为环形缓冲区
     *
     * 注意：本实现当缓冲区已满时，会将队头元素出队再从队尾插入元素，因此无论插入多少元素，都不会抛出异常
     *
     * 当缓冲区为空，再弹出元素时会报错
     */
    template<class T>
    class CircularBuffer final : public NonCopyable {
    private:
        const size_t capacity;
        int front, rear;
        T *arr;

        //TODO: for debug only!
        friend void println(const CircularBuffer &buffer) {
            for (int i = buffer.front; i != buffer.rear; i = (i + 1) % buffer.capacity) {
                printf("%d ", buffer.arr[i]);
            }
            printf("\n");
        }

    public:
        explicit CircularBuffer(size_t max_size) : capacity(max_size + 1), front(0), rear(0) {
            arr = new T[capacity];
        }

        ~CircularBuffer() {
            delete[] arr;
        }

        /**
         * 向循环队列中插入一个元素，如果当前队列已满，则将队头元素出队再插入队尾元素
         * @param x 插入元素
         */
        void push_back(const T &x) {
            arr[rear] = x;
            if (full()) front = (front + 1) % capacity;
            rear = (rear + 1) % capacity;
        }

        /**
         * 队头元素出队
         */
        T pop_front() {
            if (unlikely(empty())) FATAL << "empty!";
            T val(move(arr[front]));
            front = (front + 1) % capacity;
            return val;
        }

        [[nodiscard]]
        bool empty() const {
            return front == rear;
        }

        [[nodiscard]]
        bool full() const {
            return (rear + 1) % capacity == front;
        }

        [[nodiscard]]
        size_t size() const {
            return (rear + capacity - front) % capacity;
        }

        /**
         * 返回循环队列的容量
         * @return 容量
         */
        [[nodiscard]]
        size_t get_capacity() const {
            return capacity - 1;
        }
    };
}

#endif //REACTOR_CIRCULARBUFFER_H
