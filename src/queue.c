#include "queue.h"
#include <stdlib.h>

// Структура для очереди с приоритетом (определена ТОЛЬКО здесь)
struct Queue {
    Node* nodes[QUEUE_SIZE];
    int size;
};

// Обмен узлов
static void swapNodes(Node** first, Node** second)
{
    Node* tempNode = *first;
    *first = *second;
    *second = tempNode;
}

// Поднятие элемента вверх
static void heapUp(struct Queue* q, int index)
{
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (getNodeFrequency(q->nodes[parent]) <= getNodeFrequency(q->nodes[index]))
            break;
        swapNodes(&q->nodes[parent], &q->nodes[index]);
        index = parent;
    }
}

// Опускание элемента вниз
static void heapDown(struct Queue* q, int index)
{
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < q->size && getNodeFrequency(q->nodes[left]) < getNodeFrequency(q->nodes[smallest]))
            smallest = left;
        if (right < q->size && getNodeFrequency(q->nodes[right]) < getNodeFrequency(q->nodes[smallest]))
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

int isQueueEmpty(Queue* q)
{
    return q->size == 0;
}

int getQueueSize(Queue* q)
{
    return q->size;
}

void freeQueue(Queue* q)
{
    while (q->size > 0) {
        Node* node = dequeue(q);
        freeTree(node);
    }
}
