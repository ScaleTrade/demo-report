cmake_minimum_required(VERSION 3.16)
project(DemoReport)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Production mode
set(CMAKE_CXX_FLAGS "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS   "-O2 -DNDEBUG")
set(CMAKE_BUILD_TYPE Release)


# Подключаем локальные библиотеки
include_directories(${CMAKE_SOURCE_DIR}/includes)

add_library(DemoReport SHARED report.cpp
        ast.hpp)
target_compile_options(DemoReport PRIVATE)

target_link_libraries(DemoReport)