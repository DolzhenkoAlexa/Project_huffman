#include "huffman.h"
#include <stdio.h>
#include <string.h>

void printUsage(const char* programName)
{
    printf("Huffman Archiver\n");
    printf("\n Commands:\n");
    printf("--compress   - shrink file into .huff archive\n");
    printf("--decompress - restore original file from .huff\n");
    printf("\nUsage:\n");
    printf("  %s --compress <input> <output>\n", programName);
    printf("  %s --decompress <input> <output>\n", programName);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--compress") == 0) {
        printf("Input file:  %s\n", argv[2]);
        printf("Output file: %s\n", argv[3]);
        compressFile(argv[2], argv[3]);
    } else if (strcmp(argv[1], "--decompress") == 0) {
        printf("Input file:  %s\n", argv[2]);
        printf("Output file: %s\n", argv[3]);
        decompressFile(argv[2], argv[3]);
    } else {
        printf("Error: Unknown option '%s'\n", argv[1]);
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}