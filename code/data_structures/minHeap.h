#pragma once
#include<stdio.h>
#include<stdlib.h>
#include"node.h"

typedef struct minHeap
{
    node** harr; // Pointer To The Array In Which The Heap Data Is Stored
    int capacity; // Maximum Size
    int heap_size; // Current Size
} minHeap;

minHeap* createMinHeap(int);

void destroyMinHeap(minHeap*);

int getSize(minHeap*);

int getParent(int);

int getLeft(int);

int getRight(int);

node* getMin(minHeap*);

// Maintain the min heap property from the given index
void minHeapify(minHeap*, int);

// Extract the minimum node from the min heap
node* extractMin(minHeap*);

// Insert a new node into the min heap
void insertNode(minHeap*, node*);

void printArr(minHeap*);



minHeap* createMinHeap(int cap)
{
    minHeap* newHeap = (minHeap*)malloc(sizeof(minHeap));
    newHeap->heap_size = 0;
    newHeap->capacity = cap;
    newHeap->harr = (node**)calloc(cap, sizeof(node*));
    return newHeap;
}

void destroyMinHeap(minHeap* heap)
{
    for (int i = 0; i < heap->heap_size; i++)
        free(heap->harr[i]);

    free(heap->harr);
    free(heap);
}

int getSize(minHeap* heap)
{
    return heap->heap_size;
}

int getParent(int i)
{
    return (i - 1) / 2;
}

int getLeft(int i)
{
    return 2 * i + 1;
}

int getRight(int i)
{
    return 2 * i + 2;
}

node* getMin(minHeap* heap)
{
    return heap->harr[0];
}

// Maintain the min heap property from the given index
void minHeapify(minHeap* heap, int i)
{
    // Get the indices of the left and right children of the node at index i
    int l = getLeft(i);
    int r = getRight(i);
    // Initialize the index of the smallest node to the current node
    int smallest = i;
    // If the left child is smaller than the current node, update the index of the smallest node
    if (l < heap->heap_size && compareNodes(heap->harr[l], heap->harr[i]) == -1)
        smallest = l;
    // If the right child is smaller than the smallest node, update the index of the smallest node
    if (r < heap->heap_size && compareNodes(heap->harr[r], heap->harr[smallest]) == -1)
        smallest = r;
    // If the smallest node is not the current node, swap the nodes and continue recursively
    if (smallest != i)
    {
        swap(heap->harr[i], heap->harr[smallest]);
        minHeapify(heap, smallest);
    }
}


// Extract the minimum node from the min heap
node* extractMin(minHeap* heap)
{
    // If the heap is empty, return NULL
    if (heap->heap_size <= 0)
        return NULL;
    // If the heap contains only one node, remove it and return it
    if (heap->heap_size == 1)
    {
        heap->heap_size--;
        return heap->harr[0];
    }
    // Remove the root node, replace it with the last node, and maintain the min heap property
    node* root = heap->harr[0];
    heap->harr[0] = heap->harr[heap->heap_size - 1];
    heap->heap_size--;
    minHeapify(heap, 0);
    return root;
}

// Insert a new node into the min heap
void insertNode(minHeap* heap, node* newNode)
{
    // If the heap is full, print an error message and return
    if (heap->heap_size == heap->capacity)
    {
        printf("Error: Your Heap Is Full!\n");
        return;
    }
    // Insert the new node at the end of the heap and maintain the min heap property
    heap->heap_size++;
    int i = heap->heap_size - 1;
    heap->harr[i] = newNode;
    while (i != 0 && compareNodes(heap->harr[getParent(i)], heap->harr[i]) == 1)
    {
        swap(heap->harr[i], heap->harr[getParent(i)]);
        i = getParent(i);
    }
}

void printArr(minHeap* heap)
{
    for (int i = 0; i < heap->heap_size; i++)
    {
        printNode(heap->harr[i]);
        if (i != heap->heap_size - 1)
            printf(",");
    }
    printf("\n");
}