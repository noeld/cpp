cmake_minimum_required(VERSION 3.10)
project(kmeans CXX)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES main.cpp draw.cpp draw.h point.cpp point.h group.cpp group.h timer.cpp timer.h options.cpp options.h)

#set(Boost_DEBUG 1)
set(BOOST_ROOT C:/usr/boost_1_70_0)
# set(BOOST_INCLUDEDIR C:/usr/boost_1_71_0_mgw/include/boost-1_71)
# set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

# set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
# set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_ARCHITECTURE "-x64")
#set(Boost_RELEASE_ABI_TAG "-1_71")

find_package(Boost COMPONENTS system program_options)
# find_package(Boost REQUIRED COMPONENTS system program_options)

# link_directories(C:\\usr\\boost_1_71_0\\lib\\mingw-w64_x86_64-8.1.0\\lib )

message(STATUS "Libraries " ${Boost_LIBRARIES})
message(STATUS "Includes  " ${Boost_INCLUDE_DIRS})

# include_directories(${Boost_INCLUDE_DIRS})

add_executable(kmeans ${SOURCES})
target_include_directories(kmeans PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(kmeans PUBLIC "${CMAKE_HOME_DIRECTORY}/include")   # for CPPTastFlow
target_link_libraries(kmeans gdiplus.lib ${Boost_LIBRARIES})

install(TARGETS kmeans ) 