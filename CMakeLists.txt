cmake_minimum_required(VERSION 3.10)
project(url_shorterner)

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

# Executables
add_executable(url_shorterner src/url_shorterner.cpp)
target_link_libraries(url_shorterner fmt::fmt httplib)