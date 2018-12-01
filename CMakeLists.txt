cmake_minimum_required(VERSION 3.6)
project(prolog0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(LLVM REQUIRED CONFIG)

file(GLOB SOURCE_FILES "src/*.cc")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_executable(prolog0 ${SOURCE_FILES} src/main.cpp src/ast.cc src/wam.cc src/wam.h src/inst.cc src/inst.h src/codegen.cc src/codegen.h src/util.h src/rtti.h)
