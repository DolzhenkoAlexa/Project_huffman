#pragma once
#include <stdint.h>
#include "huffmanTree.h"

// Направления обхода дерева
typedef enum {
    LEFT = 0,
    RIGHT = 1
} Direction;

// Сжатие файла
void compressFile(const char* inputPath, const char* outputPath);

// Разжатие файла
void decompressFile(const char* inputPath, const char* outputPath);

// Подсчёт частот символов в файле
void getFrequencies(const char* path, uint64_t frequencyTable[ALPHABET_SIZE], uint64_t* fileSize);
