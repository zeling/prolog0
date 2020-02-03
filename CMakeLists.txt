cmake_minimum_required(VERSION 3.6)
project(prolog0)

find_package(readline QUIET)

add_library(prolog0 STATIC
        src/ast.cc
        src/wam.cc
        src/inst.cc
        src/parser.cc
        src/scanner.cc
        src/token.cc
        src/machine.cc
        src/machine.h
        )

set_property(TARGET prolog0 PROPERTY CXX_STANDARD 17)

add_executable(prolog0-repl src/main.cc)
target_link_libraries(prolog0-repl prolog0)
if(readline_FOUND)
target_compile_definitions(prolog0-repl PRIVATE HAS_READLINE=1)
target_link_libraries(prolog0-repl readline)
endif()
set_property(TARGET prolog0-repl PROPERTY CXX_STANDARD 17)


add_executable(test_main test/test_parser.cpp test/test_main.cpp)
target_link_libraries(test_main prolog0)
set_property(TARGET test_main PROPERTY CXX_STANDARD 17)

