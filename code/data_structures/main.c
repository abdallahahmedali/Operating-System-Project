#include <stdio.h>
#include "priorityQueue.h"

int main()
{
    priorityQueue* myQueue = createPriorityQueue(5);

    // pcb p1 = {1, 3, 0, 3};
    // pcb p2 = {2, 4, 0, 4};
    // pcb p3 = {3, 5, 0, 5};
    // pcb p4 = {4, 1, 0, 2};
    // pcb p5 = {5, 2, 0, 1};

    // enqueue(myQueue, p1,p1.priority);
    // enqueue(myQueue, p2,p2.priority);
    // enqueue(myQueue, p3,p3.priority);
    // enqueue(myQueue, p4,p4.priority);
    // enqueue(myQueue, p5,p5.priority);

    // printf("Processs in priority queue:\n");
    // print(myQueue);

    // printf("Peek at Process with lowest priority:\n");
    // pcb peekProcess = peek(myQueue);
    // printf("%d\n", peekProcess.id);

    // printf("Dequeue some Proceses:\n");
    // pcb* dequeuedProcess = dequeue(myQueue);
    // for (int i = 0; i < 5; i++){
    //     printf("%d ", dequeuedProcess->id);
    //     dequeuedProcess = dequeue(myQueue);
    // }
    // printf("\n");


    destroyPriorityQueue(myQueue);

    return 0;
}