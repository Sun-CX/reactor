//
// Created by suncx on 2020/8/11.
//

#ifndef REACTOR_BOUNDEDBLOCKINGQUEUE_H
#define REACTOR_BOUNDEDBLOCKINGQUEUE_H

#include "Condition.h"
#include "CircularBuffer.h"

namespace reactor::core {
    template<class T>
    class BoundedBlockingQueue : public NonCopyable {
    private:
        mutable Mutex mutex;
        Condition not_empty;
        Condition not_full;
        CircularBuffer<T> buffer;
    public:
        explicit BoundedBlockingQueue(int max_size) : mutex(), not_empty(mutex), not_full(mutex), buffer(max_size) {}

        void push_back(const T &x) {
            MutexGuard guard(mutex);
            while (buffer.full()) not_full.wait();
            buffer.push_back(x);
            not_empty.notify();
        }

        void push_back(T &&x) {
            MutexGuard guard(mutex);
            while (buffer.full()) not_full.wait();
            buffer.push_back(move(x));
            not_empty.notify();
        }

        T pop_front() {
            MutexGuard guard(mutex);
            while (buffer.empty()) not_empty.wait();
            T front = move(buffer.pop_front());
            not_full.notify();
            return front;
        }

        bool empty() const {
            MutexGuard guard(mutex);
            return buffer.empty();
        }

        bool full() const {
            MutexGuard guard(mutex);
            return buffer.full();
        }

        size_t size() const {
            MutexGuard guard(mutex);
            return buffer.size();
        }

        size_t capacity() const {
            MutexGuard guard(mutex);
            return buffer.get_capacity();
        }
    };
}

#endif //REACTOR_BOUNDEDBLOCKINGQUEUE_H
