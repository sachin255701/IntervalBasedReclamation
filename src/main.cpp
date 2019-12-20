#include "../inc/common.h"
#include "../inc/LockFreeQueue.h"
#include "../inc/TestThread.h"
#include "../inc/IntervalBased.h"

using namespace std;

int main(int argc, char** argv) {
	if(argc != 5) {
		std::cout << "Command line parameters are not correct !!" << std::endl;
		return -1;
	}

	const int threadCount = stoi(argv[1]);
	const int iterartionsCount = stoi(argv[2]);
	const int epochFrequency = stoi(argv[3]);
	const int emptyFrequency = stoi(argv[4]);

	const int iterartionsPerThread = iterartionsCount/threadCount;
	long totalTimeElapsed = 0;

#ifdef INTERVAL_BASED
#ifndef STACK
	IntervalBased* memeoryManager = new IntervalBased(threadCount, epochFrequency, emptyFrequency);
	LockFreeQueue* object = new LockFreeQueue(memeoryManager);
#else
	IntervalBased* memeoryManager = new IntervalBased(threadCount, epochFrequency, emptyFrequency);
	LockFreeStack* object = new LockFreeStack(memeoryManager);
#endif
#else
#ifndef STACK
        LockFreeQueue* object = new LockFreeQueue();
#else
        LockFreeStack* object = new LockFreeStack();
#endif
#endif

	thread threadArr[threadCount];
	
	std::vector<TestThread*> threads;

	for (int threadIdx = 0; threadIdx < threadCount; threadIdx++) {
		threads.push_back(new TestThread(object, iterartionsPerThread, threadIdx));
		threadArr[threadIdx] = std::thread(&TestThread::runThreads, threads[threadIdx]);
	}

	for (int threadIdx = 0; threadIdx < threadCount; threadIdx++) {
		threadArr[threadIdx].join();
	}

	return 0;
}


