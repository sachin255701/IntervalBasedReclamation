#ifndef INTERVAL_BASED_H
#define INTERVAL_BASED_H


#ifdef INTERVAL_BASED
#include "common.h"
#include "node.h"
#include <climits>
#include <cstdint>


class IntervalBased {
public:
	int epochFrequency;
	int emptyFrequency;
	int threadCount;
	std::atomic<long> deleteCounts;

private:
	int* allocatedNodeCounter;
	int* retireCounter;
	std::atomic<int>* reservationArray;
	std::atomic<long> epoch;
	std::list<Node*> *retiredNodeList;

public:
	IntervalBased(int threadCount, int epochFrequency, int emptyFrequency) {
		this->deleteCounts.store(0, std::memory_order_release);
		this->threadCount = threadCount;
		this->epochFrequency = epochFrequency;
		this->emptyFrequency = emptyFrequency;
		reservationArray = new std::atomic<int>[threadCount];
		retiredNodeList = new std::list<Node*>[threadCount];
		for (int k = 0; k < threadCount; k++) {
			/* Initialize reservation for each thread to maximum value */
			reservationArray[k].store(INT_MAX, std::memory_order_release);

			/* Remove all the elements from each thread's list */
			retiredNodeList[k].clear();
		}
		
		allocatedNodeCounter = new int[threadCount];
		retireCounter = new int[threadCount];
		epoch.store(0, std::memory_order_release);
	}

	void start_operation(int threadID) {
		long epochLocal = epoch.load(std::memory_order_acquire);
		reservationArray[threadID].store(epochLocal, std::memory_order_seq_cst);
	}

	void end_operation(int threadID) {
		reservationArray[threadID].store(INT_MAX, std::memory_order_seq_cst);
	}

	void retire(Node* node, int birthEpoch, int threadID) {
		int retireEpoch = epoch.load(std::memory_order_acquire);

		/* Update retire epoch for the node */
		node->retireEpoch = retireEpoch;

		/* Push node to retire list */
		retiredNodeList[threadID].push_back(node);

		if ((retiredNodeList[threadID].size() % emptyFrequency) == 0) {
			empty(threadID);
		}
	}
	bool deleteNode(std::vector<int> threadEpoch, int birthEpoch, int retireEpoch) {
		for (int k = 0; k < threadCount; k++) {
			if ((threadEpoch[k] >= birthEpoch) && (threadEpoch[k] <= retireEpoch)) {
				return true;
			}
		}

		return false;
	}

	void empty(int threadID) {
		int counter = 0;
		std::vector<int> threadEpoch;
		
		for (int k = 0; k < threadCount; k++) {
			threadEpoch.push_back(reservationArray[k].load(std::memory_order_acquire));
		}

		std::list<Node*>* threadLoaclList = &(retiredNodeList[threadID]);
		
		for (auto object = threadLoaclList->begin(), end = threadLoaclList->end(); object != end; ) {
			Node* node = *object;
			if (!deleteNode(threadEpoch, node->birthEpoch, node->retireEpoch)) {
				object = threadLoaclList->erase(object);
				deleteCounts.fetch_add(1, std::memory_order_release);
				delete node;
			} else {
				++object;
			}
		}
	}
	

	void* alloc(int threadID) {
		allocatedNodeCounter[threadID] = allocatedNodeCounter[threadID] + 1;

		if ((allocatedNodeCounter[threadID] % epochFrequency) == 0) {
			epoch.fetch_add(1, std::memory_order_acq_rel);
		}

		Node* newNode = new Node(NULL);
		newNode->birthEpoch = epoch.load(std::memory_order_acquire);

		return (void*)newNode;
	}

	long getDeleteCounts() {
		return deleteCounts.load();
	}

};
#endif /* INTERVAL_BASED */
#endif /* INTERVAL_BASED_H */