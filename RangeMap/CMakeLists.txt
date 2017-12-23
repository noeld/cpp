cmake_minimum_required(VERSION 3.9)
project(RangeMap)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(test)

add_executable(RangeMap main.cpp CRect.cpp CRect.h Point.cpp Point.h Transform.cpp Transform.h)