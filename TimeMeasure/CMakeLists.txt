cmake_minimum_required(VERSION 3.6)
project(TimeMeasure)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp TimeMeasure.cpp TimeMeasure.h)
add_executable(TimeMeasure ${SOURCE_FILES})