cmake_minimum_required(VERSION 3.14)
project(hw2)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw2_test_array test_array.cpp Array.h ../catch.hpp)
add_executable(hw2_test_list test_list.cpp List.h ../catch.hpp)
