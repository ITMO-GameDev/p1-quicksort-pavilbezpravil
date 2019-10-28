cmake_minimum_required(VERSION 3.14)
project(algo)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw1_test hw1/test.cpp hw1/sort.h catch.hpp)
add_executable(hw2_test_array hw2/test_array.cpp hw2/Array.h catch.hpp)
add_executable(hw2_test_list hw2/test_list.cpp hw2/List.h catch.hpp)