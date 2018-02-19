cmake_minimum_required(VERSION 3.6)
project(prolog0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB SOURCE_FILES "src/*.cc")
add_executable(prolog0 ${SOURCE_FILES} src/main.cpp)