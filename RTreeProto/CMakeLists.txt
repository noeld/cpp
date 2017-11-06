#cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ..
#cmake --build .
cmake_minimum_required(VERSION 3.6)

add_subdirectory(protoindexstring)

project(RTreeProto)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Boost)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(Boost_INCLUDE_DIRECTORY)
    ADD_DEFINITIONS("-DHAS_BOOST")
ENDIF()



#set(wxWidgets_CONFIGURATION mswu)
#find_package(wxWidgets COMPONENTS core base REQUIRED)
#include(${wxWidgets_USE_FILE})

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
set(SOURCE_FILES main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
#target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
target_link_libraries(${PROJECT_NAME} boost_filesystem boost_system)
