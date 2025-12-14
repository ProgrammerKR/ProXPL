cmake_minimum_required(VERSION 3.13)
project(ProXPL)

# Enable C and C++
enable_language(C CXX)
set(CMAKE_C_STANDARD 99)
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
include_directories(include)

# Gather sources
file(GLOB_RECURSE SOURCES 
    "src/*.c"
    "src/compiler/*.cpp"  # Pickup the new backend
)

# --- Executable ---
add_executable(proxpl ${SOURCES})

# Link LLVM and standard libraries
target_link_libraries(proxpl PRIVATE ${llvm_libs})

if(UNIX)
    target_link_libraries(proxpl PRIVATE pthread dl z tinfo) 
endif()
