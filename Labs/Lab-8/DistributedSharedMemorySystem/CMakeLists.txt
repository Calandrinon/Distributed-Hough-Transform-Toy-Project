cmake_minimum_required(VERSION 3.21)
project(DistributedSharedMemorySystem)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -pthread)
include_directories(/usr/lib/x86_64-linux-gnu/openmpi/include)

add_executable(DistributedSharedMemorySystem main.cpp DistributedSharedMemoryOperations.cpp DistributedSharedMemoryOperations.h Message.cpp Message.h)
