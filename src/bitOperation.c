#include "bitOperation.h"

// Создает битовый буфер
void initBitBuffer(BitBuffer* bitBuffer) {
    bitBuffer->buffer = 0;
    bitBuffer->count = 0;
}

// Записывает один бит в файл, накапливая их в байт
void writeBit(FILE* outputFile, BitBuffer* bitBuffer, int bitValue)
{
    bitBuffer->buffer = (bitBuffer->buffer << 1) | bitValue;
    bitBuffer->count++;

    if (bitBuffer->count == 8) {
        fwrite(&bitBuffer->buffer, 1, 1, outputFile);
        bitBuffer->buffer = 0;
        bitBuffer->count = 0;
    }
}

// Записывает оставшиеся биты в файл (добив нулями до байта)
void flushBits(FILE* outputFile, BitBuffer* bitBuffer)
{
    if (bitBuffer->count > 0) {
        bitBuffer->buffer <<= (8 - bitBuffer->count);
        fwrite(&bitBuffer->buffer, 1, 1, outputFile);
        bitBuffer->buffer = 0;
        bitBuffer->count = 0;
    }
}

// Читает один бит из файла
int readBit(FILE* inputFile, BitBuffer* bitBuffer)
{
    if (bitBuffer->count == 0) {
        if (fread(&bitBuffer->buffer, 1, 1, inputFile) != 1)
            return -1;
        bitBuffer->count = 8;
    }
    int bitValue = (bitBuffer->buffer >> 7) & 1;
    bitBuffer->buffer <<= 1;
    bitBuffer->count--;
    return bitValue;
}
