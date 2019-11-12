cmake_minimum_required(VERSION 3.6)
project(sdl2mactest VERSION 0.0.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)

set(SOURCES main.cpp)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(sdl2test ${SOURCES})

target_link_libraries(sdl2test ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})

