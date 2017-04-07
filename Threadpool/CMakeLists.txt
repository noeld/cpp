cmake_minimum_required(VERSION 3.7)
project(Threadpool)

#set(CMAKE_CXX_COMPILER g++-6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os ")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Os -s -march=native -mtune=native")

set(SOURCE_FILES main.cpp Threadpool.cpp Threadpool.h)
add_executable(Threadpool ${SOURCE_FILES})
#add_library(Threadpoollib SHARED ${SOURCE_FILES})