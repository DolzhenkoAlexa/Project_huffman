cmake_minimum_required(VERSION 3.10)
project(huffman_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(src)
include_directories(tests)

add_library(huffman_objects OBJECT
        src/bitOperation.c
        src/queue.c
        src/huffmanTree.c
        src/huffman.c
)

add_executable(huffman
        src/main.c
        $<TARGET_OBJECTS:huffman_objects>
)

add_executable(testHuffman
        tests/testHuffman.c
        $<TARGET_OBJECTS:huffman_objects>
)

add_custom_target(test
        COMMAND ./testHuffman
        DEPENDS testHuffman
        COMMENT "Running Huffman tests"
)
