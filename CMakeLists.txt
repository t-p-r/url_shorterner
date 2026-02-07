cmake_minimum_required(VERSION 3.11)
project(url_shorterner CXX)

# Set output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin/lib)
set(CMAKE_CXX_STANDARD 20)

# For clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set compiler flags
if(MSVC)
    add_compile_options(/Zi)
else() # gcc, clang
    add_compile_options(-O3 -Wall -Wextra -pipe -g)
endif()

# Set lib folder
include_directories(lib)

# Dependencies - in repo
add_library(httplib STATIC lib/httplib/httplib.h lib/httplib/httplib.cc)

# Dependencies - as package
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# Our header-only libraries
add_library(random_device INTERFACE src/random_device.hpp)
add_library(link_destination INTERFACE src/link_destination.hpp)

# Our other libraries
add_library(url_id_descriptor STATIC src/url_id_descriptor.hpp src/url_id_descriptor.cpp)
target_link_libraries(url_id_descriptor PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_link_libraries(url_id_descriptor PRIVATE random_device)

add_library(url_shorterner_container STATIC src/url_shorterner_container.hpp src/url_shorterner_container.cpp)
target_link_libraries(url_shorterner_container PRIVATE url_id_descriptor link_destination random_device)

# Our executables
add_executable(url_shorterner src/main.cpp)
target_link_libraries(url_shorterner PRIVATE httplib)
target_link_libraries(url_shorterner PRIVATE fmt::fmt spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_link_libraries(url_shorterner PRIVATE url_shorterner_container)
