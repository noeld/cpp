cmake_minimum_required(VERSION 3.6)
# /Applications/CLion.app/Contents/bin/cmake/bin/cmake -DCMAKE_CXX_COMPILER=g++-7 -DCMAKE_C_COMPILER=gcc-7 ..
# /Applications/CLion.app/Contents/bin/cmake/bin/cmake --build .

project(unicode)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_COMPILER /usr/local/bin/g++-7)
# set(CMAKE_C_COMPILER /usr/local/bin/gcc-7)

set(SOURCE_FILES unicode.cpp)

add_executable(unicode ${SOURCE_FILES})
