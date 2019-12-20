#ifndef LOCKFREE_QUEUE_H
#define LOCKFREE_QUEUE_H

#include "common.h"
#include "node.h"
#include "IntervalBased.h"


class LockFreeQueue
{
public:
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
#ifdef INTERVAL_BASED
	IntervalBased* memoryManager;
#endif

#ifdef INTERVAL_BASED
	LockFreeQueue(IntervalBased* obj)
	{
		Node* node = new Node(NULL);
		memoryManager = obj;
		head.store(node);
		tail.store(node);
	
	}
#else
	LockFreeQueue()
	{
		head.store(new Node(NULL));
		tail.store(head);

	}
#endif

	void enq(int data, int threadID)
	{
		//std::cout << "Enqueue entered!" << std::endl;
#ifdef INTERVAL_BASED
		memoryManager->start_operation(threadID);
		Node* node = (Node*)memoryManager->alloc(threadID);
		node->data = data;
#else
		Node* node = new Node(data);
#endif
		while (true) {
			Node* last = tail.load();
			Node* next = (last->nextNode).load();
			if (last == tail.load()) {
				if (next == NULL) {
					if (((tail.load())->nextNode).compare_exchange_strong(next, node)) {
						tail.compare_exchange_strong(last, node);
#ifdef INTERVAL_BASED
						memoryManager->end_operation(threadID);
#endif
						return;
					}
				} else {
					tail.compare_exchange_strong(last, next);
				}
			}
		}
	}

	int deq(int threadID) {
		//std::cout << "Dequeue entered!" << std::endl;

#ifdef INTERVAL_BASED
		memoryManager->start_operation(threadID);
#endif

		while (true) {
			Node* first = head.load();
			Node* last = tail.load();
			Node* next = (first->nextNode).load();
			if (first == last) {
				if (next == NULL) {
					/*throw new exception*/
					throw "Empty Exception.";
				}
				tail.compare_exchange_strong(last, next);
			} else {
				int value = next->data;
				if (head.compare_exchange_strong(first, next)) {
#ifdef INTERVAL_BASED			
					memoryManager->retire(first, first->birthEpoch ,threadID);
					memoryManager->end_operation(threadID);
#endif
					return value;
				}

			}
		}
	}

};

#endif