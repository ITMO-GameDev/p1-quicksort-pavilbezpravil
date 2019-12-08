cmake_minimum_required(VERSION 3.14)
project(alloc)

set(CMAKE_CXX_STANDARD 14)

add_executable(test test.cpp my_alloc.h catch.hpp)