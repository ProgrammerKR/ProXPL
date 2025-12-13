cmake_minimum_required(VERSION 3.10)
project(ProXPL C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Include directories
include_directories(include)

# Source files
# Source files
file(GLOB_RECURSE SOURCES "src/*.c")

# Core library (exposed to tests and tools)
add_library(prox_core STATIC ${SOURCES})
target_include_directories(prox_core PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

# Executable
add_executable(prox src/main.c)
target_link_libraries(prox PRIVATE prox_core)

# Optional targets: tools and tests
add_subdirectory(tests)
add_subdirectory(tools/bench)

# Tests
enable_testing()
add_subdirectory(tests)
