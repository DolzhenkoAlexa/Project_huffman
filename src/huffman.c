#include "huffman.h"
#include "bitOperation.h"
#include "huffmanTree.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void getFrequencies(const char* path, uint64_t frequencyTable[ALPHABET_SIZE], uint64_t* fileSize)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        printf("Error: Cannot open file");
        return;
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
}

// Сжимает файл
void compressFile(const char* inputPath, const char* outputPath)
{
    uint64_t frequencyTable[ALPHABET_SIZE];
    uint64_t fileSize = 0;
    getFrequencies(inputPath, frequencyTable, &fileSize);

    if (fileSize == 0) {
        FILE* out = fopen(outputPath, "wb");
        if (out != NULL) {
            fclose(out);
        }
        return;
    }

    FILE* in = fopen(inputPath, "rb");
    if (in == NULL) {
        printf("Error: Cannot open input file\n");
        return;
    }

    FILE* out = fopen(outputPath, "wb");
    if (out == NULL) {
        printf("Error: Cannot create output file\n");
        fclose(in);
        return;
    }

    Node* root = buildTree(frequencyTable);
    if (root == NULL) {
        printf("Error: Failed to build Huffman tree\n");
        fclose(in);
        fclose(out);
        return;
    }

    generateCodes(root);

    uint8_t marker = 0xFF;
    if (fwrite(&marker, 1, 1, out) != 1) {
        printf("Error: Failed to write marker\n");
        freeTree(root);
        fclose(in);
        fclose(out);
        return;
    }
    // Подсчитываем количество уникальных символов в файле
    int uniqueSymbols = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (frequencyTable[i] > 0) {
            uniqueSymbols++;
        }
    }

    if (fwrite(&uniqueSymbols, sizeof(int), 1, out) != 1) {
        printf("Error: Failed to write unique symbols count\n");
        freeTree(root);
        fclose(in);
        fclose(out);
        return;
    }
    // Таблица частот
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (frequencyTable[i] > 0) {
            unsigned char symbol = (unsigned char)i;
            if (fwrite(&symbol, 1, 1, out) != 1 || fwrite(&frequencyTable[i], sizeof(uint64_t), 1, out) != 1) {
                printf("Error: Failed to write frequency data\n");
                freeTree(root);
                fclose(in);
                fclose(out);
                return;
            }
        }
    }
    // Записываем размер исходного файла
    if (fwrite(&fileSize, sizeof(uint64_t), 1, out) != 1) {
        printf("Error: Failed to write file size\n");
        freeTree(root);
        fclose(in);
        fclose(out);
        return;
    }

    BitBuffer bitBuf;
    initBitBuffer(&bitBuf);

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        const char* code = findCode(root, (unsigned char)ch);
        if (code == NULL) {
            printf("Error: Code not found for symbol\n");
            freeTree(root);
            fclose(in);
            fclose(out);
            return;
        }
        for (size_t i = 0; i < strlen(code); i++) {
            writeBit(out, &bitBuf, code[i] == '0' ? LEFT : RIGHT);
        }
    }
    flushBits(out, &bitBuf);

    fclose(in);
    fclose(out);
    freeTree(root);
}

// Распаковка файла
void decompressFile(const char* inputPath, const char* outputPath)
{
    FILE* in = fopen(inputPath, "rb");
    if (in == NULL) {
        printf("Error: Cannot open compressed file");
        return;
    }

    uint8_t marker;
    if (fread(&marker, sizeof(uint8_t), 1, in) != 1) {
        printf("Error: Failed to read marker\n");
        fclose(in);
        return;
    }

    if (marker != 0xFF) {
        printf("Error: Invalid format (expected 0xFF, got 0x%02X)\n", marker);
        fclose(in);
        return;
    }

    int uniqueSymbols;
    if (fread(&uniqueSymbols, sizeof(int), 1, in) != 1) {
        printf("Error: Failed to read number of unique symbols\n");
        fclose(in);
        return;
    }
    // Восстанавливаем таблицу частот
    uint64_t frequencyTable[ALPHABET_SIZE] = { 0 };
    for (int i = 0; i < uniqueSymbols; i++) {
        unsigned char symbol;
        uint64_t frequency;

        if (fread(&symbol, 1, 1, in) != 1) {
            printf("Error: Failed to read symbol\n");
            fclose(in);
            return;
        }

        if (fread(&frequency, sizeof(uint64_t), 1, in) != 1) {
            printf("Error: Failed to read frequency\n");
            fclose(in);
            return;
        }

        frequencyTable[symbol] = frequency;
    }

    uint64_t originalSize;
    if (fread(&originalSize, sizeof(uint64_t), 1, in) != 1) {
        printf("Error: Failed to read file size\n");
        fclose(in);
        return;
    }

    Node* root = buildTree(frequencyTable);
    if (root == NULL) {
        printf("Error: Failed to build Huffman tree\n");
        fclose(in);
        return;
    }

    FILE* out = fopen(outputPath, "wb");
    if (out == NULL) {
        printf("Error: Cannot create output file\n");
        fclose(in);
        freeTree(root);
        return;
    }

    BitBuffer bitBuf;
    initBitBuffer(&bitBuf);

    Node* current = root;
    uint64_t decodedBytes = 0;

    while (decodedBytes < originalSize) {
        int bit = readBit(in, &bitBuf);
        if (bit == -1) {
            printf("Error: Unexpected end of file\n");
            break;
        }

        if (bit == LEFT) {
            if (current->left)
                current = current->left;
            else {
                printf("Error: invalid bit sequence\n");
                break;
            }
        } else if (bit == RIGHT) {
            if (current->right)
                current = current->right;
            else {
                printf("Error: invalid bit sequence\n");
                break;
            }
        }

        // Если достигли листа записываем символ
        if (current->left == NULL && current->right == NULL) {
            if (fputc(current->symbol, out) == EOF) {
                printf("Error: Failed to write output file\n");
                break;
            }
            decodedBytes++;
            current = root; // Возвращаемся к корню для следующего символа
        }
    }
    // Проверяем, не осталось ли лишних битов
    if (bitBuf.count > 0) {
        unsigned char remaining = bitBuf.buffer;
        remaining <<= (8 - bitBuf.count);
        if (remaining != 0) {
            puts("Warning: extra non-zero bits were found");
        }
    }

    fclose(in);
    fclose(out);
    freeTree(root);
}
