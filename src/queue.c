#include "queue.h"
#include <stdlib.h>

// Обмен узлов
static void swapNodes(Node** first, Node** second)
{
    Node* tempNode = *first;
    *first = *second;
    *second = tempNode;
}

// Поднятие элемента вверх
static void heapUp(Queue* q, int index)
{
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (q->nodes[parent]->frequency <= q->nodes[index]->frequency)
            break;
        swapNodes(&q->nodes[parent], &q->nodes[index]);
        index = parent;
    }
}

// Опускание элемента вниз
static void heapDown(Queue* q, int index)
{
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < q->size && q->nodes[left]->frequency < q->nodes[smallest]->frequency)
            smallest = left;
        if (right < q->size && q->nodes[right]->frequency < q->nodes[smallest]->frequency)
            smallest = right;

        if (smallest == index)
            break;
        swapNodes(&q->nodes[index], &q->nodes[smallest]);
        index = smallest;
    }
}

void initQueue(Queue* q)
{
    q->size = 0;
}

void enqueue(Queue* q, Node* node)
{
    q->nodes[q->size] = node;
    q->size++;
    heapUp(q, q->size - 1);
}

Node* dequeue(Queue* q)
{
    if (q->size == 0)
        return NULL;

    Node* result = q->nodes[0];
    q->nodes[0] = q->nodes[q->size - 1];
    q->size--;
    if (q->size > 0)
        heapDown(q, 0);
    return result;
}

int isEmpty(Queue* q)
{
    return q->size == 0;
}

int getQueueSize(Queue* q)
{
    return q->size;
}

int isQueueEmpty(Queue* q)
{
    return q->size == 0;
}

void freeQueue(Queue* q)
{
    while (q->size > 0) {
        Node* node = dequeue(q);
        freeTree(node);
    }
}
