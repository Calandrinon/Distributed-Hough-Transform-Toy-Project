cmake_minimum_required(VERSION 3.20)
project(MultithreadedMatrixMultiplication)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(MultithreadedMatrixMultiplication main.cpp thread_pool.h)
