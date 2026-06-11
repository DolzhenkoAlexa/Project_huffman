#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compressFile(const char* inputPath, const char* outputPath);
void decompressFile(const char* inputPath, const char* outputPath);

int compareFiles(const char* firstFilePath, const char* secondFilePath)
{
    FILE* firstFile = fopen(firstFilePath, "rb");
    FILE* secondFile = fopen(secondFilePath, "rb");

    if (!firstFile || !secondFile) {
        if (firstFile)
            fclose(firstFile);
        if (secondFile)
            fclose(secondFile);
        return 0;
    }

    int characterFromFirstFile;
    int characterFromSecondFile;

    while (1) {
        characterFromFirstFile = fgetc(firstFile);
        characterFromSecondFile = fgetc(secondFile);
        if (characterFromFirstFile != characterFromSecondFile) {
            fclose(firstFile);
            fclose(secondFile);
            return 0;
        }

        if (characterFromFirstFile == EOF) {
            break;
        }
    }
    fclose(firstFile);
    fclose(secondFile);
    return 1;
}

int testHelloWorld()
{
    printf("\nTest 1: Hello world\n");

    const char* inputFile = "tests/test1.txt";
    const char* compressedFile = "tests/test1compressed.huff";
    const char* outputFile = "tests/test1restored.txt";

    compressFile(inputFile, compressedFile);
    decompressFile(compressedFile, outputFile);

    int result = compareFiles(inputFile, outputFile);
    if (result) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    return result;
}

int testAAAAA()
{
    printf("\nTest 2: A\n");

    const char* inputFile = "tests/test2.txt";
    const char* compressedFile = "tests/test2compressed.huff";
    const char* outputFile = "tests/test2restored.txt";

    compressFile(inputFile, compressedFile);
    decompressFile(compressedFile, outputFile);

    int result = compareFiles(inputFile, outputFile);
    if (result) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    return result;
}

int testEnglish()
{
    printf("\nTest 3: English\n");

    const char* inputFile = "tests/test3.txt";
    const char* compressedFile = "tests/test3compressed.huff";
    const char* outputFile = "tests/test3restored.txt";

    compressFile(inputFile, compressedFile);
    decompressFile(compressedFile, outputFile);

    int result = compareFiles(inputFile, outputFile);
    if (result) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    return result;
}

int testRussian()
{
    printf("\nTest 4: Russian\n");

    const char* inputFile = "tests/test4.txt";
    const char* compressedFile = "tests/test4compressed.huff";
    const char* outputFile = "tests/test4restored.txt";

    compressFile(inputFile, compressedFile);
    decompressFile(compressedFile, outputFile);

    int result = compareFiles(inputFile, outputFile);
    if (result) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    return result;
}

int testLanguages()
{
    printf("\nTest 5: 3 languages\n");

    const char* inputFile = "tests/test5.txt";
    const char* compressedFile = "tests/test5compressed.huff";
    const char* outputFile = "tests/test5restored.txt";

    compressFile(inputFile, compressedFile);
    decompressFile(compressedFile, outputFile);

    int result = compareFiles(inputFile, outputFile);
    if (result) {
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    return result;
}

int main(int argc, char* argv[])
{
    printf("Running tests:\n");
    testHelloWorld();
    testAAAAA();
    testEnglish();
    testRussian();
    testLanguages();

    return 0;
}