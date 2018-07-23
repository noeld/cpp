cmake_minimum_required(VERSION 3.6)
project(Experiments)

set(CMAKE_CXX_STANDARD 14)

add_executable(deleter deleter.cpp)

add_executable(hashjoin hashjoin.cpp)

#add_executable(kdtree kdtree.cpp)

add_executable(placement placement.cpp)

add_executable(b64lookup b64lookup.cpp)

add_executable(literal literal.cpp)

add_executable(btree btree.cpp)

add_executable(hashfunction hashfunction.cpp)

add_executable(raw raw.cpp)

add_executable(gamma gamma.cpp) 

add_executable(printbits printbits.cpp) 

add_executable(zhang zhang.cpp) 