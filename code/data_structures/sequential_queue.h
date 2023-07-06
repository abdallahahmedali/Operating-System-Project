#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct anode
{
    void* value;
    struct anode* next; 
} anode;

typedef struct Queue
{
    struct anode* head;
    struct anode* tail;
    int count;
} Queue;


Queue* Queue_Init()
{
    Queue* Q = (Queue*)malloc(sizeof(Queue));
    Q->head = Q->tail = NULL;
    Q->count = 0;
    return Q;
}

bool queue_push(Queue* Q, void* value)
{
    if (Q->head == NULL)
    {
        Q->head = (struct anode*) malloc(sizeof(struct anode));
        Q->head->value = value;
        Q->tail = Q->head;
        Q->head->next = NULL;
    }
    else
    {
        struct anode* new_node = (struct anode*) malloc(sizeof(struct anode));
        new_node->value = value;
        Q->tail->next = new_node;
        Q->tail = new_node;
        Q->tail->next = NULL;
    }
    Q->count++;

    return true;
}

void* queue_top(Queue* Q)
{
    if (Q->head == NULL) return NULL;
    return Q->head->value;
}

bool queue_pop(Queue* Q)
{
    if (Q->head == NULL)
    {
        return false;
    }

    struct anode* dummy = Q->head;
    Q->head = Q->head->next;
    free(dummy);
    Q->count--;
    if (Q->head == NULL) Q->tail == NULL;
}


bool Queue_Finalize(Queue* Q)
{
    while (Q->head != NULL)
    {
        struct anode* n = Q->head;
        Q->head = Q->head->next;
        free(n);
    }
    free(Q);
    return true;
}


int queue_size(Queue* Q)
{
    return Q->count;
}


bool queue_is_empty(Queue* Q)
{
    return Q->count == 0;
}

Queue* queue_deep_copy(Queue* Q)
{
    printf("Queue Count = %d\n", Q->count);
    Queue* new_queue = Queue_Init();
    struct anode* current = Q->head;
    while (current != NULL)
    {
        struct anode* new_node = (struct anode*) malloc(sizeof(struct anode));
        new_node->value = malloc(sizeof(*(current->value))); // Allocate memory for the value
        //memcpy(new_node->value, current->value, sizeof(*(current->value))); // Copy the value contents
        
        new_node->value = (pcb*)malloc(sizeof(pcb));
        *((pcb*) new_node->value) = *( (pcb*) current->value); 
        new_node->next = NULL;

        if (new_queue->tail == NULL) // If the new queue is empty
        {
            new_queue->head = new_queue->tail = new_node;
        }
        else
        {
            new_queue->tail->next = new_node;
            new_queue->tail = new_node;
        }

        current = current->next;
    }

    new_queue->count = Q->count;

    return new_queue;
}