cmake_minimum_required(VERSION 3.15)
project(ProXPL VERSION 1.2.0 LANGUAGES C CXX)

# Enable C and C++
enable_language(C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4 /wd4244 /wd4245 /wd4267 /wd4100)
else()
  add_compile_options(-Wall -Wextra -Wno-sign-compare -Wno-unused-variable -Wno-unused-parameter)
endif()

# --- LLVM Configuration ---
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# Add explicit hints for LLVM on Windows
if(WIN32)
    # First, verify if LLVM_DIR is set and valid
    if(LLVM_DIR AND NOT EXISTS "${LLVM_DIR}/LLVMConfig.cmake")
        message(WARNING "LLVM_DIR is set to '${LLVM_DIR}' but LLVMConfig.cmake not found there. Searching for LLVM...")
        unset(LLVM_DIR CACHE)
    endif()
    
    # If LLVM_DIR is not set or invalid, search common paths
    if(NOT LLVM_DIR)
        message(STATUS "Searching for LLVM on Windows...")
        set(LLVM_ROOT_HINTS
            "$ENV{LLVM_ROOT}"
            "$ENV{ProgramFiles}/LLVM"
            "$ENV{ProgramFiles(x86)}/LLVM"
            "C:/Program Files/LLVM"
            "C:/Program Files (x86)/LLVM"
            "C:/ProgramData/chocolatey/lib/llvm/tools/llvm"
        )

        set(LLVM_SEARCH_PATHS
            "$ENV{LLVM_DIR}"
        )

        foreach(llvm_root ${LLVM_ROOT_HINTS})
            if(llvm_root)
                list(APPEND CMAKE_PREFIX_PATH "${llvm_root}")
                list(APPEND LLVM_SEARCH_PATHS
                    "${llvm_root}/lib/cmake/llvm"
                    "${llvm_root}/lib/llvm/cmake"
                    "${llvm_root}/share/llvm/cmake"
                    "${llvm_root}/cmake"
                )
            endif()
        endforeach()

        list(REMOVE_DUPLICATES LLVM_SEARCH_PATHS)
        
        foreach(search_path ${LLVM_SEARCH_PATHS})
            if(search_path AND EXISTS "${search_path}/LLVMConfig.cmake")
                set(LLVM_DIR "${search_path}" CACHE PATH "LLVM CMake directory")
                message(STATUS "Found LLVM CMake files at: ${search_path}")
                break()
            endif()
        endforeach()
    endif()
endif()

# --- LLVM Configuration ---
if(WIN32)
    # Try to find LLVM, but don't require it on Windows
    find_package(LLVM CONFIG QUIET)
else()
    # LLVM is required on Unix platforms
    find_package(LLVM CONFIG REQUIRED)
endif()

if(NOT LLVM_FOUND)
    if(WIN32)
        message(WARNING "LLVM not found. Building without LLVM backend support on Windows.")
    else()
        message(FATAL_ERROR "LLVM not found! Please install LLVM and set LLVM_DIR.")
    endif()
else()
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "LLVM Include Dirs: ${LLVM_INCLUDE_DIRS}")
    message(STATUS "LLVM Library Dirs: ${LLVM_LIBRARY_DIRS}")
    message(STATUS "LLVM Definitions: ${LLVM_DEFINITIONS}")
    add_definitions(-DUSE_LLVM_BACKEND)
endif()

# --- LibFFI Configuration ---
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBFFI libffi)
endif()

if(NOT LIBFFI_FOUND)
    find_path(LIBFFI_INCLUDE_DIRS ffi.h)
    find_library(LIBFFI_LIBRARIES NAMES ffi libffi)
    if(LIBFFI_INCLUDE_DIRS AND LIBFFI_LIBRARIES)
        set(LIBFFI_FOUND TRUE)
    endif()
endif()

if(LIBFFI_FOUND)
    message(STATUS "Found LibFFI: ${LIBFFI_LIBRARIES}")
    include_directories(${LIBFFI_INCLUDE_DIRS})
    add_definitions(-DUSE_LIBFFI)
else()
    message(WARNING "LibFFI not found. FFI features will be disabled.")
endif()

if(LLVM_FOUND)
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    link_directories(${LLVM_LIBRARY_DIRS})
    llvm_map_components_to_libnames(llvm_libs core support executionengine native ipo analysis transformutils bitwriter)
endif()

# Export symbols for Windows DLL
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# --- Include Directories ---
include_directories(include)
include_directories(src)

# List of all C/C++ source files for the library, excluding those with a 'main' function
file(GLOB_RECURSE LIB_SOURCES 
    "src/*.c" 
    "src/*.cpp"
)

# Explicitly exclude files that contain a 'main()' function or are specific VM implementations
list(REMOVE_ITEM LIB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_v2.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_register.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/instr_handlers_template.c"
)

# --- Build Configuration ---
option(PROX_STATIC_BUILD "Build as a single static executable" ON)

# Define the core library that all tools link against
add_library(prox_core STATIC ${LIB_SOURCES})
target_include_directories(prox_core PUBLIC include src)

if (LLVM_FOUND)
    target_link_libraries(prox_core PUBLIC ${llvm_libs})
endif()
if (LIBFFI_FOUND)
    target_link_libraries(prox_core PUBLIC ${LIBFFI_LIBRARIES})
endif()

if(PROX_STATIC_BUILD)
    add_definitions(-DPROX_STATIC)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        add_compile_options($<$<CONFIG:Release>:/MT> $<$<CONFIG:Debug>:/MTd>)
    endif()
endif()

# Main executable
add_executable(proxpl src/main.c)
target_link_libraries(proxpl PRIVATE prox_core)

if(UNIX)
    if(APPLE)
        target_link_libraries(proxpl PRIVATE pthread dl z)
    else()
        target_link_libraries(proxpl PRIVATE pthread dl z tinfo)
    endif()
endif()

# --- IR Test Executable ---
add_executable(ir_gen_test tools/ir_gen_test.c)
target_link_libraries(ir_gen_test PRIVATE prox_core)

# --- LLVM Gen Test Executable ---
add_executable(llvm_gen_test tools/llvm_gen_test.c)
target_link_libraries(llvm_gen_test PRIVATE prox_core)

# --- PRM Executable ---
add_executable(prm tools/prm_main.c)
target_link_libraries(prm PRIVATE prox_core)
if(UNIX)
    if(APPLE)
        target_link_libraries(prm PRIVATE pthread dl z)
    else()
        target_link_libraries(prm PRIVATE pthread dl z tinfo)
    endif()
endif()

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
