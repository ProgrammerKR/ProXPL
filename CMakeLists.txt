cmake_minimum_required(VERSION 3.15)
project(ProXPL VERSION 1.2.0 LANGUAGES C CXX)

# Enable C and C++
enable_language(C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4 /wd4244 /wd4245 /wd4267 /wd4100 /wd4200)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
else()
  add_compile_options(-Wall -Wextra -Wno-sign-compare -Wno-unused-variable -Wno-unused-parameter)
endif()

# --- Build Configuration ---
option(PROX_STATIC_BUILD "Build as a single static executable" ON)

if(PROX_STATIC_BUILD)
    add_definitions(-DPROX_STATIC)
    if(MSVC)
        # Set dynamic/static CRT for the whole project before targets are added
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        # Also add flags for older CMake versions or explicit control
        add_compile_options($<$<CONFIG:Release>:/MT> $<$<CONFIG:Debug>:/MTd>)
    endif()
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
            "$ENV{ProgramFiles\(x86\)}/LLVM"
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
    
    # Fallback for Windows if config not found (common with official installers)
    if(NOT LLVM_FOUND)
        message(STATUS "LLVM Config not found. Attempting manual detection via llvm-config...")
        find_program(LLVM_CONFIG_EXE NAMES llvm-config llvm-config.exe
            HINTS 
                "${LLVM_ROOT}/bin" 
                "$ENV{LLVM_ROOT}/bin"
                "C:/LLVM/bin"
                "C:/Program Files/LLVM/bin"
        )
        
        if(LLVM_CONFIG_EXE)
            message(STATUS "Found llvm-config at: ${LLVM_CONFIG_EXE}")
            execute_process(COMMAND ${LLVM_CONFIG_EXE} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
            execute_process(COMMAND ${LLVM_CONFIG_EXE} --libdir OUTPUT_VARIABLE LLVM_LIBRARY_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
            execute_process(COMMAND ${LLVM_CONFIG_EXE} --version OUTPUT_VARIABLE LLVM_PACKAGE_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
            
            # Convert to CMake paths
            file(TO_CMAKE_PATH "${LLVM_INCLUDE_DIRS}" LLVM_INCLUDE_DIRS)
            file(TO_CMAKE_PATH "${LLVM_LIBRARY_DIRS}" LLVM_LIBRARY_DIRS)
            
            set(LLVM_FOUND TRUE)
            set(LLVM_VERSION ${LLVM_PACKAGE_VERSION})
        endif()
    endif()
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
    # Ensure LLVM_INCLUDE_DIRS is set (sometimes it's LLVM_INCLUDE_DIR)
    if(NOT LLVM_INCLUDE_DIRS AND LLVM_INCLUDE_DIR)
        set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIR})
    endif()

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "LLVM Include Dirs: ${LLVM_INCLUDE_DIRS}")
    message(STATUS "LLVM Library Dirs: ${LLVM_LIBRARY_DIRS}")
    message(STATUS "LLVM Definitions: ${LLVM_DEFINITIONS}")
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
    if(COMMAND llvm_map_components_to_libnames)
        llvm_map_components_to_libnames(llvm_libs core support executionengine native ipo analysis transformutils bitwriter)
    elseif(LLVM_CONFIG_EXE)
        # Fallback for when LLVMConfig.cmake is missing but llvm-config works
        execute_process(COMMAND ${LLVM_CONFIG_EXE} --libnames core support executionengine native ipo analysis transformutils bitwriter 
            OUTPUT_VARIABLE llvm_libs OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(REPLACE " " ";" llvm_libs "${llvm_libs}")
    else()
        # Final fallback: just try common library names
        set(llvm_libs LLVMCore LLVMSupport LLVMExecutionEngine LLVMAnalysis LLVMTarget)
    endif()
endif()

# Export symbols for Windows DLL
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# --- Include Directories ---
include_directories(include)
include_directories(src)

# List of all C/C++ source files for the library, excluding those with a 'main' function
file(GLOB_RECURSE LIB_SOURCES "src/*.c")
file(GLOB_RECURSE LIB_SOURCES_CPP "src/*.cpp")

# Handle LLVM Backend Source: Only include if LLVM was found
if(LLVM_FOUND)
    message(STATUS "LLVM backend enabled - including backend_llvm.cpp")
else()
    message(STATUS "LLVM backend disabled - excluding backend_llvm.cpp")
    list(REMOVE_ITEM LIB_SOURCES_CPP "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/backend_llvm.cpp")
endif()

list(APPEND LIB_SOURCES ${LIB_SOURCES_CPP})

# Explicitly exclude files that contain a 'main()' function or are specific VM implementations
list(REMOVE_ITEM LIB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_v2.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_register.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/instr_handlers_template.c"
)

# Define the core library that all tools link against
add_library(prox_core STATIC ${LIB_SOURCES})
target_include_directories(prox_core PUBLIC include src)

if (LLVM_FOUND)
    target_include_directories(prox_core PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(prox_core PRIVATE USE_LLVM_BACKEND)
    # Apply LLVM compile definitions globally as some headers might need them
    add_definitions(${LLVM_DEFINITIONS})
    target_link_libraries(prox_core PUBLIC ${llvm_libs})
endif()
if (LIBFFI_FOUND)
    target_link_libraries(prox_core PUBLIC ${LIBFFI_LIBRARIES})
endif()

# Aggressive optimizations for GCC/Clang in Release mode
if(NOT MSVC)
    target_compile_options(prox_core PRIVATE $<$<CONFIG:Release>:-O3 -march=native -flto>)
    if(NOT APPLE)
        target_link_options(prox_core PUBLIC $<$<CONFIG:Release>:-flto>)
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
