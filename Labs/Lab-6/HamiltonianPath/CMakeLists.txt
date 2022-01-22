cmake_minimum_required(VERSION 3.20)
project(HamiltonianPath)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(HamiltonianPath main.cpp Graph.cpp Graph.h HamiltonianCycleDetector.cpp HamiltonianCycleDetector.h)
