cmake_minimum_required(VERSION 3.15)
project(ProXPL C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# ===============================
# Sources (exclude main.c)
# ===============================
file(GLOB_RECURSE PROX_SOURCES "src/*.c")
list(REMOVE_ITEM PROX_SOURCES "${CMAKE_SOURCE_DIR}/src/main.c")

# ===============================
# Core library (NO system libs)
# ===============================
add_library(prox_core STATIC ${PROX_SOURCES})
target_include_directories(prox_core PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

# ===============================
# Executable (link system libs HERE)
# ===============================
add_executable(prox src/main.c)

if (UNIX)
  target_link_libraries(prox PRIVATE prox_core m)
else()
  target_link_libraries(prox PRIVATE prox_core)
endif()

# ===============================
# Optional components
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
