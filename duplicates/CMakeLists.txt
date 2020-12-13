# CMakeList.txt: CMake-Projekt für "duplicates". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)

project ("duplicates" VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_TOOLCHAIN_FILE C:/usr/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-DUNICODE -D_UNICODE)

find_path(Boost_FORMAT_PATH "boost/format.hpp")
message(INFO "Boost Format " ${Boost_FORMAT_PATH})

find_package(Boost REQUIRED COMPONENTS iostreams filesystem)
message(INFO "Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")

#find_package(iconv REQUIRED)
#find_package(xxHash CONFIG REQUIRED)
#find_package(Boost REQUIRED COMPONENTS interprocess)

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (duplicates "duplicates.cpp" "duplicates.h" "ResTempl1.rct")
target_include_directories(duplicates PRIVATE ${Boost_FORMAT_PATH})
target_include_directories(duplicates PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(duplicates PUBLIC Boost::filesystem Boost::iostreams)
#target_link_libraries(duplicates PRIVATE xxHash::xxhash)

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
install(TARGETS duplicates RUNTIME DESTINATION bin)
# install(FILES file(GET_RUNTIME_DEPENDENCIES) )