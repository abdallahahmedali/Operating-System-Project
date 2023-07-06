#pragma once
#include"node.h"
#include"minHeap.h"

typedef struct priorityQueue {
    minHeap* myQueue;
} priorityQueue;

// Create a new priority queue with the given capacity
priorityQueue* createPriorityQueue(int);

// Free the memory used by the priority queue and its min heap
void destroyPriorityQueue(priorityQueue*);

// Enqueue a new process with the given priority into the priority queue
void enqueue(priorityQueue*, pcb*, int);

// Enqueue a pre-allocated node into the priority queue
void enqueueNode(priorityQueue*, node*);

// Dequeue the process with the lowest priority from the priority queue
pcb* dequeue(priorityQueue*);

// Get the process with the lowest priority from the priority queue without removing it
pcb* peek(priorityQueue*);

// Get the number of processs currently in the priority queue
int getCount(priorityQueue*);

// Print the processs in the priority queue in the order of their priorities
void print(priorityQueue*);


// Create a new priority queue with the given capacity
priorityQueue* createPriorityQueue(int cap)
{
    priorityQueue* newQueue = (priorityQueue*)malloc(sizeof(priorityQueue));
    newQueue->myQueue = createMinHeap(cap);
    return newQueue;
}

// Free the memory used by the priority queue and its min heap
void destroyPriorityQueue(priorityQueue* queue)
{
    destroyMinHeap(queue->myQueue);
    free(queue);
}

// Enqueue a new process with the given priority into the priority queue
void enqueue(priorityQueue* queue, pcb* process, int priority)
{

    node* newNode = createNewNode(process, priority);
    insertNode(queue->myQueue, newNode);

    // return &newNode->process;
}

void decreaseKey(priorityQueue* queue){
    if(getSize(queue->myQueue) == 0)
        return;
    queue->myQueue->harr[0]->priority--;
}
void decreaseSpecificKey(priorityQueue* queue, int id){
    if(getSize(queue->myQueue) == 0)
        return;
    for (size_t i = 0; i < getSize(queue->myQueue); i++)
        if(queue->myQueue->harr[i]->process->id == id)
            queue->myQueue->harr[i]->priority--;    
}
void setSpecificKeyValue(priorityQueue* queue, int id, int value){
    if(getSize(queue->myQueue) == 0)
        return;
    for (size_t i = 0; i < getSize(queue->myQueue); i++)
        if(queue->myQueue->harr[i]->process->id == id)
            queue->myQueue->harr[i]->priority = value;    
}
void decreaseSecondKey(priorityQueue* queue){
    if(getSize(queue->myQueue) <= 1)return;
    else if(getSize(queue->myQueue) == 2) {
        queue->myQueue->harr[1]->priority--;
    }else if(getSize(queue->myQueue) > 2){
        (queue->myQueue->harr[1]->priority < queue->myQueue->harr[2]->priority)?
        queue->myQueue->harr[1]->priority-- : queue->myQueue->harr[1]->priority--;
    }
}
void decreaseKeyValue(priorityQueue* queue, int val){
    if(getSize(queue->myQueue) == 0)
        return;
    queue->myQueue->harr[0]->priority = val;
}
// Enqueue a pre-allocated node into the priority queue
void enqueueNode(priorityQueue* queue, node* newNode)
{
    insertNode(queue->myQueue, newNode);
}

// Dequeue the process with the lowest priority from the priority queue
pcb* dequeue(priorityQueue* queue)
{
    node* extractedNode = extractMin(queue->myQueue);
    if (!extractedNode) {
        return (pcb*)NULL;
    }
    else
        return extractedNode->process;
}

// Get the process with the lowest priority from the priority queue without removing it
pcb* peek(priorityQueue* queue)
{
    node* minNode = getMin(queue->myQueue);
    if (!minNode || getSize(queue->myQueue) == 0) {
        return (pcb*)NULL;
    }
    else
        return minNode->process;
}

// Get the process with the second lowest priority from the priority queue without removing it
pcb* peekSecond(priorityQueue* queue)
{
    node* minNode = getMin(queue->myQueue);
    if (getSize(queue->myQueue) <= 1) {
        return (pcb*)NULL;
    }else if(getSize(queue->myQueue) == 2) {
        return queue->myQueue->harr[1]->process;
    }else if(getSize(queue->myQueue) > 2){
        return (queue->myQueue->harr[1]->priority < queue->myQueue->harr[2]->priority)?
        queue->myQueue->harr[1]->process : queue->myQueue->harr[2]->process;
    }

}

// Get the number of processs currently in the priority queue
int getCount(priorityQueue* queue)
{
    return getSize(queue->myQueue);
}

// Get front priority
int getPriority(priorityQueue* queue)
{
    node* minNode = getMin(queue->myQueue);
    if (!minNode || getSize(queue->myQueue) == 0) {
        printf("Error : Can't Peek , The Queue Is Already Empty !\n");
        return -1;
    }
    else
        return minNode->priority;
}

// Print the processs in the priority queue in the order of their priorities
void print(priorityQueue* queue)
{
    printArr(queue->myQueue);
}