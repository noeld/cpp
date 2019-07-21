cmake_minimum_required(VERSION 3.6)
project(Experiments)

set(CMAKE_CXX_STANDARD 17)

add_executable(deleter deleter.cpp)

add_executable(hashjoin hashjoin.cpp)

add_executable(placement placement.cpp)

add_executable(b64lookup b64lookup.cpp)

add_executable(literal literal.cpp)

add_executable(btree btree.cpp)

add_executable(hashfunction hashfunction.cpp)

add_executable(raw raw.cpp)

add_executable(gamma gamma.cpp) 

add_executable(printbits printbits.cpp) 

add_executable(zhang zhang.cpp) 

find_package(SDL2 REQUIRED)
# find_package(CAIRO REQUIRED)

add_executable(sdltest sdltest.cpp)
target_include_directories(sdltest PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(sdltest ${SDL2_LIBRARIES})
target_link_libraries(sdltest cairo)

find_package(Boost REQUIRED)
add_executable(indexfile indexfile.cpp)
target_link_libraries(indexfile ${BOOST_LIBRARIES})

add_executable(areagrid areagrid.cpp)


#add_compile_options("-fopenmp")
add_executable(mmap mmap.cpp)

#add_executable(mmapmp mmap.cpp)
#target_compile_options(mmapmp PUBLIC "-fopenmp" "-march=native")
#target_compile_definitions(mmapmp PUBLIC _GLIBCXX_PARALLEL)
#target_link_libraries(mmapmp gomp)

add_executable(simplebtree simplebtree.cpp)

add_executable(kdtree2 kdtree2.cpp)

add_executable(roman roman.cpp)

#add_executable(pcretest pcrecpp.cpp)
#target_include_directories(pcretest PRIVATE /usr/local/Cellar/pcre++/0.9.5/include)
#target_link_libraries(pcretest -L/usr/local/Cellar/pcre/8.43/lib -lpcre)
#target_link_libraries(pcretest -L/usr/local/Cellar/pcre++/0.9.5/lib pcre++)

add_executable(iban iban.cpp)
target_link_libraries(iban gmp)