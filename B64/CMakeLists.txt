cmake_minimum_required(VERSION 3.7)
project(B64)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Base64.cpp Base64.h)
add_executable(B64 ${SOURCE_FILES})