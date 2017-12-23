cmake_minimum_required(VERSION 3.8)
project(RTree)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp Vec2D.cpp Vec2D.h)
add_executable(RTree ${SOURCE_FILES})