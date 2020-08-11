//
// Created by ysm on 8/11/20.
//

#ifndef REACTOR_CIRCUITBUFFER_H
#define REACTOR_CIRCUITBUFFER_H

#include <memory>

template <typename T>
class CircuitBuffer {
public:
    CircuitBuffer(int n)
    :capacity_(n)
    {
        buffer = std::make_unique<T[]>(n);
    }

    ~CircuitBuffer()
    {
    }

    bool push(const T& v)
    {
        buffer[wp] = v;
        if(wp + 1 == capacity_)
            ++circle;
        wp = (wp + 1) % capacity_;
        ++size_;
        return true;
    }

    bool pop(T& dst)
    {
        if(circle == 0 && rp == wp)
            return false;
        dst = buffer[rp];
        if(rp + 1 == capacity_)
            --circle;
        rp = (rp + 1) % capacity_;
        --size_;
        return true;
    }

    bool empty()
    {
        return(circle == 0 && rp == wp);
    }
private:
    // max no. of elements that circuit buffer can hold, and real size of circuit buffer
    int capacity_ {0};
    int size_ {0};

    // wirte position index, read position index
    int wp {0};
    int rp {0};

    int circle {0};

    std::unique_ptr<T[]> buffer;
};


#endif //REACTOR_CIRCUITBUFFER_H
