cmake_minimum_required(VERSION 3.10)
project(huffman_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(huffman
        src/main.c
        src/huffman.c
        src/huffman.c
)

add_executable(testHuffman
        tests/testHuffman.c
        src/huffman.c
        src/huffman.c
)
