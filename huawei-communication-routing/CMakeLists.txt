cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wpedantic -Werror")

project(compute CXX)

file(GLOB_RECURSE SOURCES . src/main.cpp)
file(GLOB_RECURSE HEADERS . src/*.hpp)
add_executable(compute ${SOURCES})

target_include_directories(compute PUBLIC "./src")

# Linters

set(HEADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(SOURCES_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

message(STATUS "Sources directory: ${SOURCES_DIR}")
