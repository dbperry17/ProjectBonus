cmake_minimum_required(VERSION 3.6)
project(ProjectBonus)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES pgir.cpp parser.cc compiler.cc lexer.cc ir_debug.cc)
add_executable(ProjectBonus ${SOURCE_FILES})