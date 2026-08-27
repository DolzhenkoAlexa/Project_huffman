#include <stdio.h>
#include <sys/stat.h>

void compressFile(const char* inputPath, const char* outputPath);
void decompressFile(const char* inputPath, const char* outputPath);

// Получает размер файла в байтах
long getFileSize(const char* filePath)
{
    struct stat st;
    if (stat(filePath, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

// Сравнивает два файла
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

void testCompression(const char* inputFile, const char* compressedFile, const char* outputFile, const char* testName)
{
    printf("\n%s\n", testName);

    long originalSize = getFileSize(inputFile);
    if (originalSize < 0) {
        printf("ERROR: Cannot open input file '%s'\n", inputFile);
        return;
    }
    printf("Original size: %ld bytes\n", originalSize);

    compressFile(inputFile, compressedFile);

    long compressedSize = getFileSize(compressedFile);
    if (compressedSize < 0) {
        printf("ERROR: Cannot open compressed file '%s'\n", compressedFile);
        return;
    }
    printf("Compressed size: %ld bytes\n", compressedSize);

    decompressFile(compressedFile, outputFile);

    int isIdentical = compareFiles(inputFile, outputFile);
    if (isIdentical) {
        printf("Success: files are identical\n");
    } else {
        printf("FAILED: files are different\n");
    }

    if (compressedSize > 0) {
        double compressionRatio = (double)compressedSize / originalSize * 100;
        double savedPercent = 100 - compressionRatio;

        printf("Compression ratio: %.2f%% (of original)\n", compressionRatio);
        printf("Space saved: %.2f%%\n", savedPercent);

        if (savedPercent > 0) {
            printf("PASSED (saved %.2f%%)\n", savedPercent);
        } else if (savedPercent < 0) {
            printf("FAILED (increased by %.2f%%)\n", -savedPercent);
        } else {
            printf("Warning: SAME SIZE\n");
        }
    } else {
        printf("Result: EMPTY FILE\n");
    }
}

int testHelloWorld()
{
    const char* inputFile = "tests/test1.txt";
    const char* compressedFile = "tests/test1compressed.huff";
    const char* outputFile = "tests/test1restored.txt";
    testCompression(inputFile, compressedFile, outputFile, "Test 1: Hello world");
    return 1;
}

int testAAAAA()
{
    const char* inputFile = "tests/test2.txt";
    const char* compressedFile = "tests/test2compressed.huff";
    const char* outputFile = "tests/test2restored.txt";
    testCompression(inputFile, compressedFile, outputFile, "Test 2: Single repeated character");
    return 1;
}

int testEnglish()
{
    const char* inputFile = "tests/test3.txt";
    const char* compressedFile = "tests/test3compressed.huff";
    const char* outputFile = "tests/test3restored.txt";
    testCompression(inputFile, compressedFile, outputFile, "Test 3: English text");
    return 1;
}

int testRussian()
{
    const char* inputFile = "tests/test4.txt";
    const char* compressedFile = "tests/test4compressed.huff";
    const char* outputFile = "tests/test4restored.txt";
    testCompression(inputFile, compressedFile, outputFile, "Test 4: Russian text");
    return 1;
}

int testLanguages()
{
    const char* inputFile = "tests/test5.txt";
    const char* compressedFile = "tests/test5compressed.huff";
    const char* outputFile = "tests/test5restored.txt";
    testCompression(inputFile, compressedFile, outputFile, "Test 5: Multiple languages");
    return 1;
}

int main(void)
{
    printf("Running tests:\n");

    testHelloWorld();
    testAAAAA();
    testEnglish();
    testRussian();
    testLanguages();

    return 0;
}
