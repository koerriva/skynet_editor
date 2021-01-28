cmake_minimum_required(VERSION 3.17)
project(skynet_editor)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
include_directories(lib/raylib/src)
include_directories(include)

add_executable(skynet_editor main.cpp include/mycolor.h)

find_package(spdlog REQUIRED)

target_link_libraries(skynet_editor PRIVATE spdlog::spdlog raylib)