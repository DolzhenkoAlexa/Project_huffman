#include "bitOperation.h"
#include <stdlib.h>

struct BitBuffer {
    unsigned char buffer;
    int count;
};

BitBuffer* createBitBuffer(void)
{
    BitBuffer* bitBuffer = malloc(sizeof(BitBuffer));
    if (bitBuffer == NULL) {
        return NULL;
    }
    bitBuffer->buffer = 0;
    bitBuffer->count = 0;
    return bitBuffer;
}

void destroyBitBuffer(BitBuffer* bitBuffer)
{
    free(bitBuffer);
}

int writeBit(FILE* outputFile, BitBuffer* bitBuffer, int bitValue)
{
    bitBuffer->buffer = (bitBuffer->buffer << 1) | bitValue;
    bitBuffer->count++;

    if (bitBuffer->count == 8) {
        if (fwrite(&bitBuffer->buffer, 1, 1, outputFile) != 1) {
            return -3;
        }
        bitBuffer->buffer = 0;
        bitBuffer->count = 0;
    }
    return 0;
}

int flushBits(FILE* outputFile, BitBuffer* bitBuffer)
{
    if (bitBuffer->count > 0) {
        bitBuffer->buffer <<= (8 - bitBuffer->count);
        if (fwrite(&bitBuffer->buffer, 1, 1, outputFile) != 1) {
            return -3;
        }
        bitBuffer->buffer = 0;
        bitBuffer->count = 0;
    }
    return 0;
}

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

int getRemainingBits(const BitBuffer* bitBuffer, unsigned char* remaining)
{
    if (bitBuffer->count == 0) {
        return 0;
    }
    *remaining = bitBuffer->buffer << (8 - bitBuffer->count);
    return bitBuffer->count;
}
