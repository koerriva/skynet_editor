cmake_minimum_required(VERSION 3.0)
project(skynet_editor)

set(CMAKE_CXX_STANDARD 17)

# 设置源文件目录
set (PROJECT_SOURCE_DIR src)

#set(CMAKE_CXX_FLAGS "-Wno-c++11-narrowing")
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(src DIR_SRCS)
aux_source_directory(src/engine DIR_SRCS)
aux_source_directory(src/core DIR_SRCS)
aux_source_directory(src/brain DIR_SRCS)

add_subdirectory(lib)

include_directories(src)
include_directories(lib/raylib/src)
include_directories(lib/raylib/src/raygui/src)
include_directories(lib/lua/include)
include_directories(lib/entt/src)
include_directories(lib/glm)
include_directories(lib/SDL2/include)

file(COPY data DESTINATION .)

add_executable(skynet_editor src/main.cpp ${DIR_SRCS} src/brain/world.cpp src/brain/editor.cpp src/brain/menu.cpp src/brain/bug.cpp src/brain/graphics/lighting.h src/core/GraphEditor.cpp src/core/GraphEditor.h src/brain/heartchart.h)

#find_package(spdlog REQUIRED)

target_link_libraries(skynet_editor raylib lua_static)