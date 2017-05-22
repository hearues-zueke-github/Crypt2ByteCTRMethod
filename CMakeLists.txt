cmake_minimum_required(VERSION 3.3)
project(EnDeCryptionHZMethod)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp source/test_print_block.c include/test_print_block.h source/test_crypt_2B_block.c include/test_crypt_2B_block.h)
add_executable(EnDeCryptionHZMethod ${SOURCE_FILES})