cmake_minimum_required(VERSION 3.31)
project(teammitc)

set(CMAKE_CXX_STANDARD 26)

add_executable(teammitc src/main.cpp
        src/tokenization.h
)