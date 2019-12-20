# Interval Based Reclamation
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;One of the major concerns for Lock-free data structures is the reclamation of memory occupied by removed nodes. Unmanaged languages that do not have automatic garbage collection support, Safe Memory Reclamation (SMR) techniques are designed to ensures that memory of removed nodes is freed (for reuse or returned to the OS) only when no other thread is having reference to the removed node along with providing lock-free progress guaranteed by the data structure. C and C++ are examples of such un managed programming languages. These languages don’t have inbuilt capability of garbage collection.  

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Interval Based Reclamation (IBR) scheme is derived from Epoch Based Reclamation (EBR) scheme. In EBR scheme each thread tries to reserve all memory blocks that are created after a particular time, however IBR scheme only reserves a set of blocks that are created between a particular time interval. IBR approach is enhancement over EBR approach as this approach provides efficient solution of stalled threads as well. In EBR, if a thread gets stalled, it may prevent other threads from reclaiming the memory blocks that were created after a particular time point, since this algorithm tends to reserve all the memory blocks after a particular point of time. So, even a single thread stall might lead to memory exhaustion. IBR technique, fortunately, resolves this problem, as it only reserves certain set of memory blocks, which allows other threads to free the memory blocks which are not reserved by stalled thread.

## Compilation and Execution
How to compile and execute the project on UNIX system:
1. Goto src/ directory and type following commnd in shell:
   ```
   g++ main.cpp -o main  -std=c++11 -lpthread  
   ```
   Above command will create an executable named main inside source directory.

2. To simply run the program, type following command:
   ```
   ./main <thread count> <iterations> <epoch frequency> <empty frequency>
   ```
   
3. If you want to run the program with memory profiler, type following command:
   ```
   valgrind --tool=massif ./main <thread count> <iterations> <epoch frequency> <empty frequency>  
   ```
   Tool, after completing the execution will generate a report file with name which will appear something like  massif.out.<id>. id is printed every time tool exits the execution.


## Directory Structure:
1. /src/main.cpp: File which is responsible to take inputs from user. This file requires four inputs: a. Thread Count b. Iteration Count c. Epoch Frequency d. Empty Frequency.

2. /inc/TestThread.h: Header file which is responsible for execution of each thread. Each thread executes the runThreads method in this file.

3. /inc/node.h: Header file which defines the memory node structure.

4. /inc/common.h: This file is used to include common header file. There are two major macros in this file:  
A. INTERVAL_BASED: This macro must me defined in order to run IBR scheme for memory reclamation. If not defined, the program will run the data structures without any SMR scheme.  
B. STACK: This macro must be defined in order to select Stack as operating data structure on which relamation scheme is applied. If not defined, by default, program will pick Queue as default data structure.

5. /inc/LockFreeQueue.h: C++ implementation of Lock Free Queue.

6. /inc/LockFreeStack.h: C++ implementation of Lock Free Stack.

7. /inc/IntervalBased.h: IBR scheme is implemented inside this header file.


# References
[1] Haosen Wen, Joseph Izraelevitz, Wentao Cai, H. Alan Beadle, and Michael L. Scott, "Interval-Based Memory Reclamation", PPoPP ’18, February 24–28, 2018, Vienna, Austria.
