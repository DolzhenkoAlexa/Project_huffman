#pragma once
#include "huffmanTree.h"

#define QUEUE_SIZE 512

// Структура для очереди с приоритетом
typedef struct {
    Node* nodes[QUEUE_SIZE];
    int size;
} Queue;

// Инициализирует очередь
void initQueue(Queue* queue);

// Добавляет узел в очередь
void enqueue(Queue* queue, Node* node);

// Извлекает узел с наименьшей частотой
Node* dequeue(Queue* queue);

// Проверяет, пуста ли очередь
int isQueueEmpty(Queue* queue);

// Получает размер очереди
int getQueueSize(Queue* queue);

// Освобождает все узлы в очереди
void freeQueue(Queue* queue);
