cmake_minimum_required(VERSION 3.10)
project(SevenWondersDuel)

# 设置 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 让编译器去 src 目录下找头文件
include_directories(src)

# 递归搜索 src 文件夹下所有的 .cpp 文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 生成程序
add_executable(SevenWondersDuel ${SOURCES})