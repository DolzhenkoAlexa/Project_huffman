#include "huffman.h"
#include "bitOperation.h"
#include "huffmanTree.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int getFrequencies(const char* path, uint64_t frequencyTable[ALPHABET_SIZE], uint64_t* fileSize)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        return -1;
    }
    *fileSize = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        frequencyTable[i] = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        frequencyTable[(unsigned char)ch]++;
        (*fileSize)++;
    }
    fclose(f);
    return 0;
}

int compressFile(const char* inputPath, const char* outputPath)
{
    uint64_t frequencyTable[ALPHABET_SIZE];
    uint64_t fileSize = 0;

    int result = getFrequencies(inputPath, frequencyTable, &fileSize);
    if (result != 0) {
        return result;
    }

    if (fileSize == 0) {
        FILE* out = fopen(outputPath, "wb");
        if (out != NULL) {
            fclose(out);
        }
        return 0;
    }

    FILE* in = fopen(inputPath, "rb");
    if (in == NULL) {
        return -1;
    }

    FILE* out = fopen(outputPath, "wb");
    if (out == NULL) {
        fclose(in);
        return -1;
    }

    Node* root = buildTree(frequencyTable);
    if (root == NULL) {
        fclose(in);
        fclose(out);
        return -2;
    }

    generateCodes(root);

    uint8_t marker = 0xFF;
    if (fwrite(&marker, 1, 1, out) != 1) {
        freeTree(root);
        fclose(in);
        fclose(out);
        return -3;
    }

    // Подсчитываем количество уникальных символов в файле
    int uniqueSymbols = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (frequencyTable[i] > 0) {
            uniqueSymbols++;
        }
    }

    if (fwrite(&uniqueSymbols, sizeof(int), 1, out) != 1) {
        freeTree(root);
        fclose(in);
        fclose(out);
        return -3;
    }

    // Таблица частот
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (frequencyTable[i] > 0) {
            unsigned char symbol = (unsigned char)i;
            if (fwrite(&symbol, 1, 1, out) != 1 || fwrite(&frequencyTable[i], sizeof(uint64_t), 1, out) != 1) {
                freeTree(root);
                fclose(in);
                fclose(out);
                return -3;
            }
        }
    }

    // Записываем размер исходного файла
    if (fwrite(&fileSize, sizeof(uint64_t), 1, out) != 1) {
        freeTree(root);
        fclose(in);
        fclose(out);
        return -3;
    }

    BitBuffer* bitBuf = createBitBuffer();
    if (bitBuf == NULL) {
        freeTree(root);
        fclose(in);
        fclose(out);
        return -2;
    }

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        const char* code = findCode(root, (unsigned char)ch);
        if (code == NULL) {
            freeTree(root);
            fclose(in);
            fclose(out);
            destroyBitBuffer(bitBuf);
            return -2;
        }
        for (size_t i = 0; i < strlen(code); i++) {
            result = writeBit(out, bitBuf, code[i] == '0' ? LEFT : RIGHT);
            if (result != 0) {
                freeTree(root);
                fclose(in);
                fclose(out);
                destroyBitBuffer(bitBuf);
                return result;
            }
        }
    }

    result = flushBits(out, bitBuf);
    if (result != 0) {
        freeTree(root);
        fclose(in);
        fclose(out);
        destroyBitBuffer(bitBuf);
        return result;
    }

    destroyBitBuffer(bitBuf);

    fclose(in);
    fclose(out);
    freeTree(root);
    return 0;
}

int decompressFile(const char* inputPath, const char* outputPath)
{
    FILE* in = fopen(inputPath, "rb");
    if (in == NULL) {
        return -1;
    }

    uint8_t marker;
    if (fread(&marker, sizeof(uint8_t), 1, in) != 1) {
        fclose(in);
        return -1;
    }

    if (marker != 0xFF) {
        fclose(in);
        return -1;
    }

    int uniqueSymbols;
    if (fread(&uniqueSymbols, sizeof(int), 1, in) != 1) {
        fclose(in);
        return -1;
    }

    // Восстанавливаем таблицу частот
    uint64_t frequencyTable[ALPHABET_SIZE] = { 0 };
    for (int i = 0; i < uniqueSymbols; i++) {
        unsigned char symbol;
        uint64_t frequency;

        if (fread(&symbol, 1, 1, in) != 1) {
            fclose(in);
            return -1;
        }

        if (fread(&frequency, sizeof(uint64_t), 1, in) != 1) {
            fclose(in);
            return -1;
        }

        frequencyTable[symbol] = frequency;
    }

    uint64_t originalSize;
    if (fread(&originalSize, sizeof(uint64_t), 1, in) != 1) {
        fclose(in);
        return -1;
    }

    Node* root = buildTree(frequencyTable);
    if (root == NULL) {
        fclose(in);
        return -2;
    }

    FILE* out = fopen(outputPath, "wb");
    if (out == NULL) {
        fclose(in);
        freeTree(root);
        return -1;
    }

    BitBuffer* bitBuf = createBitBuffer();
    if (bitBuf == NULL) {
        fclose(in);
        freeTree(root);
        fclose(out);
        return -2;
    }

    Node* current = root;
    uint64_t decodedBytes = 0;

    while (decodedBytes < originalSize) {
        int bit = readBit(in, bitBuf);
        if (bit == -1) {
            break;
        }

        unsigned char symbol;
        int status = decodeSymbol(&current, bit, &symbol);

        if (status == -3) {
            // Битовая последовательность не соответствует дереву
            fclose(in);
            fclose(out);
            freeTree(root);
            destroyBitBuffer(bitBuf);
            return -3;
        } else if (status == 1) {
            // Символ получен
            if (fputc(symbol, out) == EOF) {
                fclose(in);
                fclose(out);
                freeTree(root);
                destroyBitBuffer(bitBuf);
                return -3;
            }
            decodedBytes++;
            current = root; // Возврат к корню для следующего символа
        }
        // Продолжаем читать биты
    }

    unsigned char remaining;
    if (getRemainingBits(bitBuf, &remaining) > 0) {
        if (remaining != 0) {
            puts("Warning: extra non-zero bits were found");
        }
    }

    fclose(in);
    fclose(out);
    freeTree(root);
    return 0;
}
