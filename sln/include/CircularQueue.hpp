#ifndef CIRCULAR_QUEUE_HPP_
#define CIRCULAR_QUEUE_HPP_

#include <vector>

namespace cq {

bool isEmpty(int head, int tail) {
    return head == tail;
}

bool isFull(int head, int tail, const int N) {
    return ((head + 1) % N) == tail;
}

template <class T, size_t N>
bool dequeue(T(&&t)[N], int& head, int& tail) {
    if (isEmpty(head, tail))
        return false;
    tail = (tail + 1) % N;
    return true;
}

template <class T, size_t N>
bool enqueue(T(&&t)[N], T item, int& head, int& tail) {
    if (isFull(head, tail, N))
        return false;  // dequeue(t, head, tail);
    if (head == -1)
        tail = 0;
    head = (head + 1) % N;
    t[head] = item;
    return true;
}

template <class T, size_t N>
std::vector<T> tokenise(T(&&t)[N], int head, int tail) {
    std::vector<T> v;
    for (int i = tail, j = 0; j < N; i++, j++)
        v.push_back(t[i % N]);
    return v;
}

}  // namespace cq

#endif  // CIRCULAR_QUEUE_HPP_
