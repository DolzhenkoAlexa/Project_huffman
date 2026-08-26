#include "huffmanTree.h"
#include "huffman.h"
#include "queue.h"
#include <stdlib.h>
#include <string.h>

struct Node {
    unsigned char symbol;
    uint64_t frequency;
    char* code;
    struct Node* left;
    struct Node* right;
};

Node* createNode(unsigned char symbol, uint64_t frequency, Node* left, Node* right)
{
    Node* node = malloc(sizeof(Node));
    if (node == NULL) {
        return NULL;
    }
    node->symbol = symbol;
    node->frequency = frequency;
    node->code = NULL;
    node->left = left;
    node->right = right;
    return node;
}

Node* buildTree(uint64_t frequencyTable[ALPHABET_SIZE])
{
    Queue queue;
    initQueue(&queue);

    // Создаем листья для всех символов с ненулевой частотой
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (frequencyTable[i] > 0) {
            unsigned char symbol = (unsigned char)i;
            Node* leaf = createNode(symbol, frequencyTable[i], NULL, NULL);
            if (leaf == NULL) {
                freeQueue(&queue);
                return NULL;
            }
            enqueue(&queue, leaf);
        }
    }

    if (isQueueEmpty(&queue)) {
        return NULL;
    }

    // Если только один символ, создает фиктивный родительский узел
    if (getQueueSize(&queue) == 1) {
        Node* leaf = dequeue(&queue);
        Node* parent = createNode(0, leaf->frequency, leaf, NULL);
        if (parent == NULL) {
            freeTree(leaf);
            return NULL;
        }
        return parent;
    }

    // Строим дерево
    while (getQueueSize(&queue) > 1) {
        Node* left = dequeue(&queue);
        Node* right = dequeue(&queue);

        uint64_t newFrequency = left->frequency + right->frequency;
        Node* parent = createNode(0, newFrequency, left, right);

        if (parent == NULL) {
            freeTree(left);
            freeTree(right);
            freeQueue(&queue);
            return NULL;
        }
        enqueue(&queue, parent);
    }

    return dequeue(&queue);
}

// Рекурсивно строит коды для всех символов
static int buildCodeRecursive(Node* root, char* code, int depth)
{
    if (root == NULL) {
        return 0;
    }

    if (root->left == NULL && root->right == NULL) {
        root->code = (char*)malloc(depth + 1);
        if (root->code == NULL) {
            return -2;
        }
        if (depth == 0) {
            root->code[0] = '0';
            root->code[1] = '\0';
        } else {
            memcpy(root->code, code, depth);
            root->code[depth] = '\0';
        }
        return 0;
    }

    // Идём налево и дописываем 0
    if (root->left) {
        code[depth] = '0';
        int result = buildCodeRecursive(root->left, code, depth + 1);
        if (result != 0) {
            return result;
        }
    }
    // Идём направо и дописываем 1
    if (root->right) {
        code[depth] = '1';
        int result = buildCodeRecursive(root->right, code, depth + 1);
        if (result != 0) {
            return result;
        }
    }

    return 0;
}

int generateCodes(Node* root)
{
    if (root == NULL) {
        return 0;
    }
    char* buffer = malloc(ALPHABET_SIZE * sizeof(char));
    if (buffer == NULL) {
        return -2;
    }
    int result = buildCodeRecursive(root, buffer, 0);
    free(buffer);
    return result;
}

const char* findCode(Node* node, unsigned char symbol)
{
    if (node == NULL) {
        return NULL;
    }

    if (node->left == NULL && node->right == NULL) {
        return (node->symbol == symbol) ? node->code : NULL;
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
    if (root == NULL)
        return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

uint64_t getNodeFrequency(const Node* node)
{
    return node->frequency;
}

int decodeSymbol(Node** current, int bit, unsigned char* symbol)
{
    Node* node = *current;
    // 0 = Left,  1 = Right
    if (bit == 0) {
        if (node->left == NULL)
            // нет левого ребенка
            return -3;
        *current = node->left;
    } else if (bit == 1) {
        if (node->right == NULL)
            // нет правого ребенка
            return -3;
        *current = node->right;
    }

    node = *current;
    if (node->left == NULL && node->right == NULL) {
        *symbol = node->symbol;
        // Получили символ
        return 1;
    }
    // Продолжаем читать биты
    return 0;
}
