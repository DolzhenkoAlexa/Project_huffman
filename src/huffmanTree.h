#pragma once

#include <stdint.h>

#define ALPHABET_SIZE 256

// Структура узла дерева Хаффмана
typedef struct Node {
    unsigned char symbol;
    uint64_t frequency;
    char* code;
    struct Node* left;
    struct Node* right;
} Node;

// Создает новый узел
Node* createNode(unsigned char symbol, uint64_t frequency, Node* left, Node* right);

// Строит дерево Хаффмана по таблице частот
Node* buildTree(uint64_t frequencyTable[ALPHABET_SIZE]);

// Генерирует коды для всех символов
void generateCodes(Node* root);

// Ищет код для символа
const char* findCode(Node* root, unsigned char symbol);

// Освобождает память дерева
void freeTree(Node* root);
