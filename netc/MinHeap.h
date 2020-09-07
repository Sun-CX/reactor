//
// Created by suncx on 2020/9/6.
//

#ifndef REACTOR_MINHEAP_H
#define REACTOR_MINHEAP_H

#include <vector>
#include <stdexcept>

using std::move;
using std::vector;
using std::logic_error;

// 要求类型 T 重载 < 运算符
template<class T, const int dimension>
class MinHeap final {
private:
    vector<T> heap;

    void sift_up(int end);

    void sift_down(int begin, int end);

    int get_min_child_idx(int parent_idx);

public:
    void insert(T &&x);
//    void insert(const T &x);

    T peek() const;

    T pop();

    bool empty() const;

    size_t size() const;
};

template<class T, const int dimension>
void MinHeap<T, dimension>::insert(T &&x) {
    heap.push_back(move(x));
    sift_up(heap.size() - 1);
}

template<class T, const int dimension>
T MinHeap<T, dimension>::pop() {
    if (empty()) throw logic_error("can not pop an empty heap!");
    T temp = move(heap.front());
    if (size() == 1) {
        heap.pop_back();
        return temp;
    }
    heap.front() = move(heap.back());
    heap.pop_back();
    sift_down(0, heap.size() - 1);
    return temp;
}

template<class T, const int dimension>
void MinHeap<T, dimension>::sift_up(int end) {
    int idx = end, parent_idx = (idx - 1) / dimension;
    T temp = move(heap[idx]);
    while (idx > 0) {
        if (temp < heap[parent_idx]) {
            heap[idx] = move(heap[parent_idx]);
            idx = parent_idx;
            parent_idx = (idx - 1) / dimension;
        } else break;
    }
    heap[idx] = move(temp);
}

template<class T, const int dimension>
void MinHeap<T, dimension>::sift_down(int begin, int end) {
    int idx = begin, child_idx = idx * dimension + 1;
    T temp = move(heap[idx]);
    while (child_idx <= end) {
        child_idx = get_min_child_idx(idx);
        if (heap[child_idx] < temp) {
            heap[idx] = move(heap[child_idx]);
            idx = child_idx;
            child_idx = idx * dimension + 1;
        } else break;

    }
    heap[idx] = move(temp);
}

template<class T, const int dimension>
int MinHeap<T, dimension>::get_min_child_idx(int parent_idx) {
    int first_child_idx = parent_idx * dimension + 1;
    int min_idx = first_child_idx;
    for (int i = first_child_idx + 1; i < first_child_idx + dimension and i < heap.size(); ++i)
        if (heap[i] < heap[min_idx]) min_idx = i;
    return min_idx;
}

template<class T, const int dimension>
bool MinHeap<T, dimension>::empty() const {
    return heap.empty();
}

template<class T, const int dimension>
size_t MinHeap<T, dimension>::size() const {
    return heap.size();
}

template<class T, const int dimension>
T MinHeap<T, dimension>::peek() const {
    if (empty()) throw logic_error("can not peek an empty heap!");
    return heap[0];
}

template<class T>
using BinaryHeap = MinHeap<T, 2>;

template<class T>
using QuadHeap = MinHeap<T, 4>;
#endif //REACTOR_MINHEAP_H
