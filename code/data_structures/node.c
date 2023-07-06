// #include"node.h"

// node* createNewNode(pcb process, int priority)
// {
//     node* newNode = (node*)malloc(sizeof(node));
//     newNode->process = process;
//     newNode->priority = priority;
//     return newNode;
// }

// void swap(node* x, node* y)
// {
//     pcb process = x->process;
//     int priority = x->priority;
//     x->process = y->process;
//     x->priority = y->priority;
//     y->process = process;
//     y->priority = priority;
// }

// int compareNodes(node* node1, node* node2)
// {
//     if (node1->priority < node2->priority)
//         return -1;
//     else if (node1->priority > node2->priority)
//         return 1;
//     else
//         return 0;
// }

// void printNode(node* node)
// {
//     if (node != NULL)
//         printf("(%d, %d)", node->process.id, node->priority);
//     else
//         printf("NULL");
// }

// void deleteNode(node** node)
// {
//     free(*node);
//     node = NULL;
// }