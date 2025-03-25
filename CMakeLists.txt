cmake_minimum_required(VERSION 3.10)
project(DynaFlow C)

set(CMAKE_C_STANDARD 11)

add_executable(dataset_gen src/dataset_gen.c)
add_executable(traditional src/traditional.c)
add_executable(hybrid_immediate src/hybrid_immediate.c)

