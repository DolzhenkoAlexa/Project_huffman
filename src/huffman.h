#pragma once
#include "huffmanTree.h"
#include <stdint.h>

// Направления обхода дерева
typedef enum {
    LEFT = 0,
    RIGHT = 1
} Direction;

// Сжатие файла
int compressFile(const char* inputPath, const char* outputPath);

// Разжатие файла
int decompressFile(const char* inputPath, const char* outputPath);

// Подсчёт частот символов в файле
int getFrequencies(const char* path, uint64_t frequencyTable[ALPHABET_SIZE], uint64_t* fileSize);
