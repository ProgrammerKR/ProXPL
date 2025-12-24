cmake_minimum_required(VERSION 3.15)
project(ProXPL)

# Enable C and C++
enable_language(C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# --- LLVM Configuration ---
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

# Map generic components to specific libs
llvm_map_components_to_libnames(llvm_libs core support executionengine native ipo)

# Export symbols for Windows DLL
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# --- Include Directories ---
include_directories(include)
include_directories(src)

# --- Library Sources ---
file(GLOB_RECURSE LIB_SOURCES 
    "src/*.c"
    "src/*.cpp"
)
# Exclude main entry point from library
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.c$")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*vm_v2\\.c$")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*vm_dispatch\\.c$")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*instr_handlers_template\\.c$")

# --- Core Library ---
add_library(proxpl_lib SHARED ${LIB_SOURCES})
target_link_libraries(proxpl_lib PRIVATE ${llvm_libs})

# --- Main Executable ---
add_executable(proxpl src/main.c)
target_link_libraries(proxpl PRIVATE proxpl_lib ${llvm_libs})

if(UNIX)
    target_link_libraries(proxpl PRIVATE pthread dl z tinfo) 
    target_link_libraries(proxpl_lib PRIVATE pthread dl z tinfo)
endif()

# --- IR Test Executable ---
add_executable(ir_gen_test tools/ir_gen_test.c)
target_link_libraries(ir_gen_test PRIVATE proxpl_lib)

# --- LLVM Gen Test Executable ---
add_executable(llvm_gen_test tools/llvm_gen_test.c)

target_link_libraries(llvm_gen_test PRIVATE proxpl_lib ${llvm_libs})

# --- PRM Executable ---
add_executable(prm 
    tools/prm_main.c 
    src/prm/manifest.c 
    src/prm/builder.c
)
# PRM doesn't strictly need proxpl_lib unless it uses core types?
# For now, it's standalone, but let's link it if needed later.
# target_link_libraries(prm PRIVATE proxpl_lib)

# --- Optional components ---
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_BENCH "Build benchmarks" OFF)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (BUILD_BENCH)
  add_subdirectory(tools/bench)
endif()
