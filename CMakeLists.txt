cmake_minimum_required(VERSION 3.10)
project(url_shorterner CXX)

# Set output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin/lib)
set(CMAKE_CXX_STANDARD 20)

# Set compiler flags
add_compile_options(-O2 -pipe -g)

# Set lib folder
include_directories(lib)

# Dependencies - installed on system
find_package(fmt)

# Dependencies - in repo
add_library(httplib STATIC lib/httplib/httplib.h lib/httplib/httplib.cc)
add_subdirectory(lib/spdlog)
find_package(spdlog)

# Our header-only libraries
add_library(random_device INTERFACE src/random_device.hpp)
add_library(link_state INTERFACE src/link_state.hpp)

# Our other libraries
add_library(url_id_descriptor STATIC src/url_id_descriptor.hpp src/url_id_descriptor.cpp)
target_link_libraries(url_id_descriptor PRIVATE random_device)

add_library(url_shorterner_container STATIC src/url_shorterner_container.hpp src/url_shorterner_container.cpp)
target_link_libraries(url_shorterner_container PRIVATE url_id_descriptor link_state random_device)

# Our executables
add_executable(url_shorterner src/main.cpp)
target_link_libraries(url_shorterner fmt::fmt httplib)
target_link_libraries(url_shorterner url_shorterner_container)
