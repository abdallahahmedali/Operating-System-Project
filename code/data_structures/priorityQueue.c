// #include"priorityQueue.h"

// // Create a new priority queue with the given capacity
// priorityQueue* createPriorityQueue(int cap)
// {
//     priorityQueue* newQueue = (priorityQueue*)malloc(sizeof(priorityQueue));
//     newQueue->myQueue = createMinHeap(cap);
//     return newQueue;
// }

// // Free the memory used by the priority queue and its min heap
// void destroyPriorityQueue(priorityQueue* queue)
// {
//     destroyMinHeap(queue->myQueue);
//     free(queue);
// }

// // Enqueue a new process with the given priority into the priority queue
// void enqueue(priorityQueue* queue, pcb process, int priority)
// {
//     node* newNode = createNewNode(process, priority);
//     insertNode(queue->myQueue, newNode);
// }

// // Enqueue a pre-allocated node into the priority queue
// void enqueueNode(priorityQueue* queue, node* newNode)
// {
//     insertNode(queue->myQueue, newNode);
// }

// // Dequeue the process with the lowest priority from the priority queue
// pcb* dequeue(priorityQueue* queue)
// {
//     node* extractedNode = extractMin(queue->myQueue);
//     if (!extractedNode) {
//         printf("Error : Can't Dequeue , The Queue Is Already Empty !\n");
//         return;
//     }
//     else
//         return &(extractedNode->process);
// }

// // Get the process with the lowest priority from the priority queue without removing it
// pcb peek(priorityQueue* queue)
// {
//     node* minNode = getMin(queue->myQueue);
//     if (!minNode || getSize(queue->myQueue) == 0) {
//         printf("Error : Can't Peek , The Queue Is Already Empty !\n");
//         return;
//     }
//     else
//         return minNode->process;
// }

// // Get the number of processs currently in the priority queue
// int getCount(priorityQueue* queue)
// {
//     return getSize(queue->myQueue);
// }

// // Print the processs in the priority queue in the order of their priorities
// void print(priorityQueue* queue)
// {
//     printArr(queue->myQueue);
// }