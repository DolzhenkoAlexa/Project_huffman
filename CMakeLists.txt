cmake_minimum_required(VERSION 3.10)
project(huffman_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)
include_directories(tests)

add_library(huffman_lib STATIC
        src/bitOperation.c
        src/queue.c
        src/huffmanTree.c
        src/huffman.c
)

add_executable(huffman src/main.c)
target_link_libraries(huffman huffman_lib)

add_executable(testHuffman tests/testHuffman.c)
target_link_libraries(testHuffman huffman_lib)
target_include_directories(testHuffman PRIVATE src)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(huffman_lib PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(huffman PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(testHuffman PRIVATE -Wall -Wextra -Wpedantic)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(huffman_lib PRIVATE /W3)
    target_compile_options(huffman PRIVATE /W3)
    target_compile_options(testHuffman PRIVATE /W3)
endif()


if(WIN32)
    add_custom_target(test
            COMMAND testHuffman.exe
            DEPENDS testHuffman
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests
            COMMENT "Running Huffman tests (Windows)"
    )
else()
    add_custom_target(test
            COMMAND ./testHuffman
            DEPENDS testHuffman
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests
            COMMENT "Running Huffman tests (Linux)"
    )
endif()
