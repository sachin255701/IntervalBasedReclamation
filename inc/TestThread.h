#ifndef TEST_THREAD_H
#define TEST_THREAD_H
#include "common.h"
#include "LockFreeQueue.h"
#include "LockFreeStack.h"

class TestThread {
	int iters;

#ifdef STACK
	LockFreeStack* stack;
#else
	LockFreeQueue* queue;
#endif

	std::atomic<long> enqueueCounts;
	std::atomic<long> dequeueCounts;
	std::chrono::milliseconds duration;

public:
	int threadId;

#ifdef STACK
	TestThread(LockFreeStack* stack, int iters, int threadID){
#else
	TestThread(LockFreeQueue* queue, int iters, int threadID){
#endif
		duration = std::chrono::milliseconds(0);
		enqueueCounts.store(0, std::memory_order_release);
		dequeueCounts.store(0, std::memory_order_release);
		this->threadId = threadID;
#ifdef STACK
		this->stack = stack;
#else
		this->queue = queue;
#endif
		this->iters = iters;
	}

	void runThreads() {
		int randomNumber;
		
		auto start = std::chrono::high_resolution_clock::now();

		for (int iter = 0; iter < this->iters; iter++) {
			if(iter% 2 == 0){
#ifdef STACK
				this->stack->push(iter, this->threadId);
#else
				this->queue->enq(iter, this->threadId);
#endif
				enqueueCounts.fetch_add(1, std::memory_order_acq_rel);
			} else {
#ifdef STACK
				this->stack->pop(this->threadId);
#else
				this->queue->deq(this->threadId);
#endif
				dequeueCounts.fetch_add(1, std::memory_order_acq_rel);
			}
		}

		auto end = std::chrono::high_resolution_clock::now();

		this->duration = (std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
	}

	long getEnqueueCounts() {
		return enqueueCounts.load();
	}

	long getDequeueCounts() {
		return dequeueCounts.load();
	}

	long getElapsedTime() {
		return duration.count();
	}
};
#endif