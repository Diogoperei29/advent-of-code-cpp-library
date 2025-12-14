cmake_minimum_required(VERSION 3.16)
project(advent_of_code_cpp_library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(aoc examples/main.cpp)
target_include_directories(aoc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)