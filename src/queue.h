#pragma once
#include "huffmanTree.h"

#define QUEUE_SIZE 512

typedef struct Queue Queue;

// Создает новую очередь
Queue* createQueue(void);

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
