#ifndef CIRCULAR_QUEUE_HPP_
#define CIRCULAR_QUEUE_HPP_

namespace cq {

	bool isEmpty(int head, int tail) {
		return head == tail;
	}

	bool isFull(int head, int tail, const int N) {
		return ((head + 1) % N) == tail;
	}

	template<class T, size_t N>
	bool dequeue(T(&&t)[N], int&& head, int&& tail) {
		if (isEmpty(head, tail))
			return false;
		tail = (tail + 1) % N;
		return true;
	}

	template<class T, size_t N>
	bool enqueue(T(&&t)[N], T item, int&& head, int&& tail) {
		if (isFull(head, tail, N))
			return false; // dequeue(t, head, tail);
		if (head == -1)
			tail = 0;
		head = (head + 1) % N;
		t[head] = item;
		return true;
	}

	template<class T, size_t N>
	void print(T(&&t)[N], int&& head, int&& tail) {
		std::cout << "(";
		for (int i = tail, j = 0; j < N; i++, j++)
			std::cout << t[i % N] << ", ";
		std::cout << ")";
	}

}  // namespace cq

#endif // CIRCULAR_QUEUE_HPP_
