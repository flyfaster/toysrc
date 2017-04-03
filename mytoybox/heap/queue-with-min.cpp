//============================================================================
// Name        : queue-with-min.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <deque>
#include <set>
#include <unordered_map>
using namespace std;

class queue_with_min {
public:
	queue_with_min(size_t capacity);
	void push(int data);
	int pop();
	int get_min();
private:
	// use a queue and heap
	std::vector<std::pair<int, int>> heap; // find min, first: data, second: index in fifo
	std::vector<std::pair<int, int>> fifo; // first: data, second: index in heap
	size_t readpos_=0;
	size_t writepos_=0;
	size_t size_=0;
};

queue_with_min::queue_with_min(size_t capacity) {
	heap.resize(capacity);
	fifo.resize(capacity);
}

void queue_with_min::push(int data) {
	//
	if(size_==heap.size()-1)
	{
		std::cout << __func__ << " full" << std::endl;
		return;
	}
	fifo[writepos_]={data, size_};
	
	heap[size_]={data, writepos_}; // last element of heap
	writepos_++; 	writepos_%=heap.size();		
	// siftUp heap[size_]
	size_t heap_pos = size_;
	while(heap_pos) {
		size_t heap_parent = heap_pos/2;
		if(heap[heap_pos]<heap[heap_parent]) {
			// update fifo heap index
			swap(fifo[heap[heap_pos].second].second, fifo[heap[heap_parent].second].second);
			swap(heap[heap_pos], heap[heap_parent]); 
			heap_pos = heap_parent;
		} else
			break;
	}
	size_++;
}

void test_queue_with_min() {
	queue_with_min qm(1024);
	qm.push(123);
	qm.get_min();
	qm.pop();
}

int main() {
	cout << "queue_with_min" << endl;
	test_queue_with_min();
	return 0;
}

inline int queue_with_min::pop() {
	if(size_ ==0)
		return 0;
	int result= fifo[readpos_].first;
	int heap_pos = fifo[readpos_].second;
	// remove heap slot of fifo[readpos_].second
	// move last slot in heap to fifo[readpos_].second

	// size_--;
	// siftDown heap[fifo[readpos_].second]
	readpos_++; readpos_%=fifo.size();
	return result;
}

inline int queue_with_min::get_min() {
	return heap[0].first;
}
