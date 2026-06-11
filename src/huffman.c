#include "huffman.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структура узла дерева Хаффмана
struct Node {
    unsigned char symbol;
    uint64_t freq;
    char* code;
    struct Node* left;
    struct Node* right;
};

static struct Node* queue[QUEUE_SIZE];
static int queueSize = 0;
static unsigned char bitBuffer = 0;
static int bitCount = 0;

// Записывает один бит в файл, накапливая их в байт
static void writeBit(FILE* out, int bit)
{
    bitBuffer = (bitBuffer << 1) | bit;
    bitCount++;

    printf("%d", bit);

    if (bitCount == 8) {
        fwrite(&bitBuffer, 1, 1, out);
        bitBuffer = 0;
        bitCount = 0;
    }
}

// Записывает оставшиеся биты в файл (добив нулями до байта)
static void flushBits(FILE* out)
{
    if (bitCount > 0) {
        bitBuffer <<= (8 - bitCount);
        fwrite(&bitBuffer, 1, 1, out);
        bitBuffer = 0;
        bitCount = 0;
    }
    printf("\n");
}

// Читает один бит из файла
static int readBit(FILE* in, unsigned char* buffer, int* bitsLeft)
{
    if (*bitsLeft == 0) {
        if (fread(buffer, 1, 1, in) != 1)
            return -1;
        *bitsLeft = 8;
    }
    int bit = ((*buffer) >> 7) & 1;
    (*buffer) <<= 1;
    (*bitsLeft)--;
    return bit;
}

// Добавляет узел в конец очереди
static void enqueue(Node* node) { queue[queueSize++] = node; }

// Ищет индекс узла с наименьшей частотой
static int findMinIndex(void)
{
    int minIndex = 0;
    for (int i = 1; i < queueSize; i++) {
        if (queue[i]->freq < queue[minIndex]->freq) {
            minIndex = i;
        } else if (queue[i]->freq == queue[minIndex]->freq) {
            if (queue[i]->symbol < queue[minIndex]->symbol) {
                minIndex = i;
            }
        }
    }
    return minIndex;
}

// Удаляет узел из очереди по индексу
static void deleteByIndex(int index)
{
    for (int i = index; i < queueSize - 1; i++) {
        queue[i] = queue[i + 1];
    }
    queueSize--;
}

// Извлекает из очереди узел с наименьшей частотой
static struct Node* dequeue(void)
{
    if (queueSize == 0)
        return NULL;
    int minIndex = findMinIndex();
    Node* minNode = queue[minIndex];
    deleteByIndex(minIndex);
    return minNode;
}

// Создаёт новый узел дерева
static Node* createNode(unsigned char symbol, uint64_t freq, struct Node* left,
    struct Node* right)
{
    Node* node = malloc(sizeof(struct Node));
    node->symbol = symbol;
    node->freq = freq;
    node->code = NULL;
    node->left = left;
    node->right = right;
    return node;
}

Node* buildTree(uint64_t freqs[ALPHABET_SIZE])
{
    queueSize = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (freqs[i] > 0) {
            unsigned char symbol = (unsigned char)i;
            Node* leaf = createNode(symbol, freqs[i], NULL, NULL);
            enqueue(leaf);
        }
    }

    if (queueSize == 0) {
        return NULL;
    }

    // Если только один символ, создает фиктивный родительский узел
    if (queueSize == 1) {
        Node* leaf = dequeue();
        Node* parent = createNode(0, leaf->freq, leaf, NULL);
        return parent;
    }

    while (queueSize > 1) {
        Node* left = dequeue();
        Node* right = dequeue();
        uint64_t newFreq = left->freq + right->freq;
        Node* parent = createNode(0, newFreq, left, right);
        enqueue(parent);
    }
    return dequeue();
}

// Рекурсивно строит коды для всех символов
static void recursiveCodeBuilding(Node* root, char* code, int depth)
{
    if (root == NULL) {
        return;
    }

    if (root->left == NULL && root->right == NULL) {
        root->code = (char*)malloc(depth + 1);
        if (depth == 0) {
            // Специальный случай с единственным символом
            root->code[0] = '0';
            root->code[1] = '\0';
        } else {
            for (int i = 0; i < depth; i++) {
                root->code[i] = code[i];
            }
            root->code[depth] = '\0';
        }
        return;
    }

    // Идём налево и дописываем 0
    if (root->left) {
        code[depth] = '0';
        recursiveCodeBuilding(root->left, code, depth + 1);
    }
    // Идём направо и дописываем 1
    if (root->right) {
        code[depth] = '1';
        recursiveCodeBuilding(root->right, code, depth + 1);
    }
}

void generateCodes(Node* root)
{
    if (root == NULL) {
        return;
    }
    char* buffer = malloc(ALPHABET_SIZE * sizeof(char));
    recursiveCodeBuilding(root, buffer, 0);
    free(buffer);
}

// Ищет код
static const char* findCode(Node* node, unsigned char symbol)
{
    if (node == NULL) {
        return NULL;
    }

    if (node->left == NULL && node->right == NULL) {
        if (node->symbol == symbol) {
            return node->code;
        }
        return NULL;
    }

    // Ищем в левом поддереве
    const char* leftCode = findCode(node->left, symbol);
    if (leftCode != NULL) {
        return leftCode;
    }

    // Ищем в правом поддереве
    return findCode(node->right, symbol);
}

void freeTree(Node* root)
{
    Node* node = root;
    if (node == NULL)
        return;
    freeTree(node->left);
    freeTree(node->right);
    if (node->code)
        free(node->code);
    free(node);
}

void getFrequencies(const char* path, uint64_t freqs[ALPHABET_SIZE],
    uint64_t* fileSize)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        printf("Error: Cannot open file");
        return;
    }
    *fileSize = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        freqs[i] = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        freqs[(unsigned char)ch]++;
        (*fileSize)++;
    }
    fclose(f);
}

// Сжимает файл
void compressFile(const char* inputPath, const char* outputPath)
{
    uint64_t freqs[ALPHABET_SIZE];
    uint64_t fileSize = 0;
    getFrequencies(inputPath, freqs, &fileSize);

    if (fileSize == 0) {
        FILE* out = fopen(outputPath, "wb");
        fclose(out);
        return;
    }

    int uniqueSymbols = 0;
    unsigned char singleSymbol = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (freqs[i] > 0) {
            uniqueSymbols++;
            singleSymbol = (unsigned char)i;
        }
    }

    FILE* in = fopen(inputPath, "rb");
    FILE* out = fopen(outputPath, "wb");

    // Один уникальный символ
    if (uniqueSymbols == 1) {
        uint8_t marker = 0x00;
        fwrite(&marker, 1, 1, out);
        fwrite(&singleSymbol, 1, 1, out);
        fwrite(&fileSize, 8, 1, out);

        fclose(in);
        fclose(out);
        return;
    }

    // Два и более символов
    uint8_t marker = 0xFF;
    fwrite(&marker, 1, 1, out);

    fwrite(&uniqueSymbols, sizeof(int), 1, out);

    // Только ненулевые частоты
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (freqs[i] > 0) {
            unsigned char symbol = (unsigned char)i;
            fwrite(&symbol, 1, 1, out);
            fwrite(&freqs[i], sizeof(uint64_t), 1, out);
        }
    }

    Node* root = buildTree(freqs);
    generateCodes(root);
    fwrite(&fileSize, sizeof(uint64_t), 1, out);
    bitBuffer = 0;
    bitCount = 0;

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        const char* code = findCode(root, (unsigned char)ch);
        for (int i = 0; i < strlen(code); i++) {
            writeBit(out, code[i] == '0' ? LEFT : RIGHT);
        }
    }
    flushBits(out);

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

    // Один уникальный символ
    if (marker == 0x00) {
        unsigned char symbol;
        uint64_t originalSize;

        if (fread(&symbol, sizeof(unsigned char), 1, in) != 1) {
            printf("Error: Failed to read symbol\n");
            fclose(in);
            return;
        }

        if (fread(&originalSize, sizeof(uint64_t), 1, in) != 1) {
            printf("Error: Failed to read file size\n");
            fclose(in);
            return;
        }

        FILE* out = fopen(outputPath, "wb");
        if (out == NULL) {
            fclose(in);
            return;
        }

        for (uint64_t i = 0; i < originalSize; i++) {
            fputc(symbol, out);
        }

        fclose(in);
        fclose(out);
        return;
    }

    // Два символа и более
    if (marker != 0xFF) {
        printf("Error: Invalid format\n");
        fclose(in);
        return;
    }

    // Читаем количество уникальных символов
    int uniqueSymbols;
    if (fread(&uniqueSymbols, sizeof(int), 1, in) != 1) {
        printf("Error: Failed to read number of unique symbols\n");
        fclose(in);
        return;
    }

    // Читаем ненулевые частоты
    uint64_t freqs[ALPHABET_SIZE] = { 0 };
    for (int i = 0; i < uniqueSymbols; i++) {
        unsigned char symbol;
        uint64_t freq;

        if (fread(&symbol, 1, 1, in) != 1) {
            printf("Error: Failed to read symbol\n");
            fclose(in);
            return;
        }

        if (fread(&freq, sizeof(uint64_t), 1, in) != 1) {
            printf("Error: Failed to read frequency\n");
            fclose(in);
            return;
        }

        freqs[symbol] = freq;
    }

    uint64_t originalSize;
    if (fread(&originalSize, sizeof(uint64_t), 1, in) != 1) {
        printf("Error: Failed to read file size\n");
        fclose(in);
        return;
    }

    Node* root = buildTree(freqs);
    if (root == NULL) {
        fclose(in);
        return;
    }

    FILE* out = fopen(outputPath, "wb");
    if (out == NULL) {
        fclose(in);
        freeTree(root);
        return;
    }

    unsigned char buffer = 0;
    int bitsLeft = 0;
    struct Node* current = root;
    uint64_t decodedBytes = 0;

    while (decodedBytes < originalSize) {
        int bit = readBit(in, &buffer, &bitsLeft);
        if (bit == -1)
            break;

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

        if (current->left == NULL && current->right == NULL) {
            fputc(current->symbol, out);
            decodedBytes++;
            current = root;
        }
    }

    if (bitsLeft > 0) {
        unsigned char remaining = buffer;
        remaining <<= (8 - bitsLeft);
        if (remaining != 0) {
            puts("Error: extra non-zero bits were found");
        }
    }

    fclose(in);
    fclose(out);
    freeTree(root);
}