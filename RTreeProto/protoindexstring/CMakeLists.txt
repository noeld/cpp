#cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ..
#cmake --build .
cmake_minimum_required(VERSION 3.6)

project(protoindexstring)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Boost)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(Boost_INCLUDE_DIRECTORY)
    ADD_DEFINITIONS("-DHAS_BOOST")
ENDIF()

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
set(SOURCE_FILES protoindexstring.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
