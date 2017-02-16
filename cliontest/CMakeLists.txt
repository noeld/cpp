cmake_minimum_required(VERSION 3.6)
project(cliontest)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Boost)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(Boost_INCLUDE_DIRECTORY)
    ADD_DEFINITIONS("-DHAS_BOOST")
ENDIF()

include(CPack)

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})


#set(CMAKE_BUILD_TYPE Release)

set(SOURCE_FILES main.cpp Endianness.cpp Endianness.h Range2D.h Mandelbrot.cpp Mandelbrot.h Vector.cpp Vector.h Planet.cpp Planet.h Universe.cpp Universe.h CLionTestApp.cpp CLionTestApp.h CLionTestFrame.cpp CLionTestFrame.h PlanetGenerator.cpp PlanetGenerator.h)
add_executable(cliontest ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
