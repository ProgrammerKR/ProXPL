cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE PROX_SRC "${CMAKE_SOURCE_DIR}/src/*.c")
add_library(prox_core OBJECT ${PROX_SRC})

add_executable(bytecode_tests bytecode_tests.c)
target_link_libraries(bytecode_tests PRIVATE prox_core)
target_include_directories(bytecode_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME BytecodeTests COMMAND bytecode_tests)
