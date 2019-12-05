# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (MagicThread)

# 添加 Magic_Engine 子目录
add_subdirectory(MagicThread)
add_subdirectory(Demo1)

set(CMAKE_BUILD_TYPE "Debug")

MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})