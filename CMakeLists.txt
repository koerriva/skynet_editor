cmake_minimum_required(VERSION 3.0)
project(skynet_editor)

set(CMAKE_CXX_STANDARD 20)

# 设置源文件目录
set (PROJECT_SOURCE_DIR src src/include/myinput.h src/include/myscript.h src/game.cpp src/gui.cpp src/include/node.h)

set(CMAKE_CXX_FLAGS "-Wno-c++11-narrowing")
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(src DIR_SRCS)

add_subdirectory(lib)
include_directories(lib/raylib/src)
include_directories(lib/raylib/src/raygui/src)
include_directories(lib/lua/include)
include_directories(src/include)
include_directories(lib/entt/src)

file(COPY data DESTINATION .)

add_executable(skynet_editor main.cpp ${PROJECT_SOURCE_DIR})

find_package(spdlog REQUIRED)

target_link_libraries(skynet_editor PRIVATE spdlog::spdlog raylib lua_static)