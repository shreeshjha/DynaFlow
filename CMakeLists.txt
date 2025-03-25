cmake_minimum_required(VERSION 3.10)
project(DynaFlow C)

# Set the C standard
set(CMAKE_C_STANDARD 11)

# Create two executable targets for the two approaches
add_executable(traditional src/traditional.c)
add_executable(hybrid_feedback src/hybrid_feedback.c)

