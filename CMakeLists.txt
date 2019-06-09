cmake_minimum_required(VERSION 3.6)
project(prolog0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_library(prolog0 STATIC
        src/ast.cc
        src/wam.cc
        src/inst.cc
        src/codegen.cc
        src/parser.cc
        src/scanner.cc
        src/token.cc
        src/machine.cc
        src/machine.h
        )

add_executable(prolog0-repl src/main.cc)
target_link_libraries(prolog0-repl prolog0 readline)

add_executable(test_main test/test_parser.cpp test/test_main.cpp test/test_codegen.cpp)
target_link_libraries(test_main prolog0)
