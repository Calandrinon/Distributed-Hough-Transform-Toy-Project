cmake_minimum_required(VERSION 3.20)
project(Producer_Consumer_Problem)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(Producer_Consumer_Problem main.cpp)
