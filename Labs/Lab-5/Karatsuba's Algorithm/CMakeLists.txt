cmake_minimum_required(VERSION 3.20)
project(Karatsuba_s_Algorithm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(Karatsuba_s_Algorithm main.cpp Polynomial.cpp Polynomial.h PolynomialArithmetic.cpp PolynomialArithmetic.h ThreadPool.h)
