#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Размер алфавита
#define ALPHABET_SIZE 256
#define QUEUE_SIZE 512

// Направления обхода дерева
typedef enum {
    LEFT = 0,
    RIGHT = 1
} Direction;

// Структура узла дерева Хаффмана
typedef struct Node Node;

// Построение дерева для Хаффмана по таблице частот
Node* buildTree(uint64_t freqs[ALPHABET_SIZE]);

// Построение кодов Хаффмана для всех символов дерева
// Коды записываются в поле code каждого узла-листа
void generateCodes(Node* root);

// Освобождение памяти дерева
void freeTree(Node* root);

// Сжатие файла
void compressFile(const char* inputPath, const char* outputPath);

// Разжатие файла
void decompressFile(const char* inputPath, const char* outputPath);

// Подсчёт частот символов в файле
void getFrequencies(const char* path, uint64_t freqs[ALPHABET_SIZE], uint64_t* fileSize);