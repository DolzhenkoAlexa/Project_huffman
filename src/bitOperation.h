#pragma once

#include <stdio.h>

// Структура для битового буфера
typedef struct {
    unsigned char buffer;
    int count;
} BitBuffer;

// Записывает один бит в файл
void writeBit(FILE* outputFile, BitBuffer* bitBuffer, int bitValue);

// Записывает оставшиеся биты в файл
void flushBits(FILE* outputFile, BitBuffer* bitBuffer);

// Читает один бит из файла
int readBit(FILE* inputFile, BitBuffer* bitBuffer);

// Создает битовый буфер
void initBitBuffer(BitBuffer* bitBuffer);
