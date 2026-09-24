#pragma once

#include <stdint.h>

#define ALPHABET_SIZE 256

// Структура узла дерева Хаффмана
typedef struct Node Node;

// Создает новый узел
Node* createNode(unsigned char symbol, uint64_t frequency, Node* left, Node* right);

// Строит дерево Хаффмана по таблице частот
Node* buildTree(uint64_t frequencyTable[ALPHABET_SIZE]);

// Генерирует коды для всех символов
int generateCodes(Node* root);

// Ищет код для символа
const char* findCode(Node* node, unsigned char symbol);

// Освобождает память дерева
void freeTree(Node* root);

// Геттер для частоты
uint64_t getNodeFrequency(const Node* node);

// Декодирует символ: возвращает 1 - символ получен, 0 - нужно продолжать, -3 - ошибка чтения
int decodeSymbol(Node** current, int bit, unsigned char* symbol);
