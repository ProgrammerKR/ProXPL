cmake_minimum_required(VERSION 3.15)
project(ProXPL C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE PROX_SOURCES "src/*.c")

# Core library
add_library(prox_core STATIC ${PROX_SOURCES})
target_include_directories(prox_core PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

# Main executable
add_executable(prox src/main.c)
target_link_libraries(prox PRIVATE prox_core)

# ===============================
# Optional components (OFF by default)
# ===============================
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_BENCH "Build benchmarks" OFF)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (BUILD_BENCH)
  add_subdirectory(tools/bench)
endif()
