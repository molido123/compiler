cmake_minimum_required(VERSION 3.26)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lexical Lexical.cpp)
add_executable(Grammer grammer.cpp)
