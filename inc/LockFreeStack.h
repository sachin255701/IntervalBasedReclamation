#ifndef LOCKFREE_STACK_H
#define LOCKFREE_STACK_H

#include "common.h"
#include "node.h"
#include "IntervalBased.h"


class LockFreeStack {
public:
	std::atomic<Node*> top;

#ifdef INTERVAL_BASED
	IntervalBased* memoryManager;

	LockFreeStack(IntervalBased* obj) {
		memoryManager = obj;
		top.store(new Node(NULL));
	}
#else
	LockFreeStack() {
		top.store(new Node(NULL));
	}
#endif
	
	void push(int data, int threadID) {
#ifdef INTERVAL_BASED
		memoryManager->start_operation(threadID);
		Node* node = (Node*)memoryManager->alloc(threadID);
#else
		Node* node = new Node(data);
#endif

		Node* oldTop = NULL;
		bool success = false;
		while (!success) {
			oldTop = top.load();
			node->nextNode.store(oldTop);
			success = top.compare_exchange_strong(oldTop, node);
		}
#ifdef INTERVAL_BASED
		memoryManager->end_operation(threadID);
#endif
	}

	int pop(int threadID) {
#ifdef INTERVAL_BASED
		memoryManager->start_operation(threadID);
#endif
		Node* oldTop = NULL;
		Node* newTop = NULL;
		bool success = false;
		while (!success) {
			oldTop = top.load();
			newTop = oldTop->nextNode.load();
			success = top.compare_exchange_strong(oldTop, newTop);
		}
#ifdef INTERVAL_BASED			
		memoryManager->retire(oldTop, oldTop->birthEpoch, threadID);
		memoryManager->end_operation(threadID);
#endif

		return oldTop->data;
	}
};

#endif /* LOCKFREE_STACK_H */