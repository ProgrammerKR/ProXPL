cmake_minimum_required(VERSION 3.10)

# Build bench_simple
add_executable(bench_simple bench_simple.c)
target_include_directories(bench_simple PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(bench_simple PRIVATE prox_core)

add_custom_target(run_bench_simple
    COMMAND bench_simple
    DEPENDS bench_simple
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
