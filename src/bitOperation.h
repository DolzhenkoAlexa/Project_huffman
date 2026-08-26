#pragma once

#include <stdio.h>

typedef struct BitBuffer BitBuffer;

// Создает битовый буфер
BitBuffer* createBitBuffer(void);

// Уничтожает битовый буфер
void destroyBitBuffer(BitBuffer* bitBuffer);

// Записывает один бит в файл
int writeBit(FILE* outputFile, BitBuffer* bitBuffer, int bitValue);

// Записывает оставшиеся биты в файл
int flushBits(FILE* outputFile, BitBuffer* bitBuffer);

// Читает один бит из файла
int readBit(FILE* inputFile, BitBuffer* bitBuffer);

// Проверяет остаточные биты и возвращает их значение
int getRemainingBits(const BitBuffer* bitBuffer, unsigned char* remaining);
