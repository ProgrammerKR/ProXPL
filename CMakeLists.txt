<<<<<<< HEAD
cmake_minimum_required(VERSION 3.15)
project(ProXPL C)
=======
cmake_minimum_required(VERSION 3.13)
project(ProXPL)
>>>>>>> feature/opcode-tests

# Enable C and C++
enable_language(C CXX)
set(CMAKE_C_STANDARD 99)
<<<<<<< HEAD
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
=======
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- LLVM Configuration ---
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Link directories
link_directories(${LLVM_LIBRARY_DIRS})

# Map generic components to specific libs
llvm_map_components_to_libnames(llvm_libs core support executionengine native ipo)

# --- Project Source ---
# Export symbols for Windows DLL
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# --- Project Source ---
include_directories(include)

# Gather sources (excluding main.c for library)
file(GLOB_RECURSE LIB_SOURCES 
    "src/*.c"
    "src/compiler/*.cpp"
)
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.c$")

# --- Shared Library ---
add_library(proxpl_lib SHARED ${LIB_SOURCES})
target_link_libraries(proxpl_lib PRIVATE ${llvm_libs})

# --- Executable ---
add_executable(proxpl src/main.c ${LIB_SOURCES})

# Link LLVM and standard libraries
target_link_libraries(proxpl PRIVATE ${llvm_libs})

if(UNIX)
    target_link_libraries(proxpl PRIVATE pthread dl z tinfo) 
    target_link_libraries(proxpl_lib PRIVATE pthread dl z tinfo)
>>>>>>> feature/opcode-tests
endif()
