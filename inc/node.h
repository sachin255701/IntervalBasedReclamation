#pragma once

class Node
{
public:
	int data;
	std::atomic<Node*> nextNode;

#ifdef INTERVAL_BASED
	int birthEpoch;
	int retireEpoch;
#endif

	Node(int data) {
		this->data = data;
		nextNode.store(NULL, std::memory_order_relaxed);
	}
};