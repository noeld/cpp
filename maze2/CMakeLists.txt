cmake_minimum_required(VERSION 3.9)
project(maze2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native -march=native -O2")

add_executable(maze2 main.cpp maze.cpp maze.h MazeBitmapPainter.cpp MazeBitmapPainter.h)
target_link_libraries(maze2 gdiplus.dll )