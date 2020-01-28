cmake_minimum_required(VERSION 3.10)
project(kmeans CXX)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES main.cpp draw.cpp draw.h point.cpp point.h group.cpp group.h)

add_executable(kmeans ${SOURCES})

target_link_libraries(kmeans gdiplus.lib)