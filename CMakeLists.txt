cmake_minimum_required(VERSION 3.15)
<<<<<<< HEAD
project(ProXPL VERSION 1.0.0 LANGUAGES C CXX)
=======
project(ProXPL)
>>>>>>> fix-ci-build

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
<<<<<<< HEAD
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# --- LLVM Configuration ---
find_package(LLVM CONFIG)

# --- LibFFI Configuration ---
# specialized find_package might be needed if not in standard path, but let's try standard first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBFFI libffi)
endif()

# Fallback or manual find if pkg-config fails or on simple windows setups
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

=======
find_package(LLVM CONFIG)

>>>>>>> fix-ci-build
if (LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    link_directories(${LLVM_LIBRARY_DIRS})

    # Map generic components to specific libs
<<<<<<< HEAD
    llvm_map_components_to_libnames(llvm_libs core support executionengine native ipo analysis transformutils bitwriter)
=======
    llvm_map_components_to_libnames(llvm_libs core support executionengine native ipo)
>>>>>>> fix-ci-build
else()
    message(WARNING "LLVM not found. Building without LLVM backend.")
    set(llvm_libs "")
endif()

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
<<<<<<< HEAD
# Exclude prototype VM register which has its own main
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*vm_register\\.c$")
=======
>>>>>>> fix-ci-build

if (NOT LLVM_FOUND)
    list(FILTER LIB_SOURCES EXCLUDE REGEX ".*backend_llvm\\.cpp$")
endif()

<<<<<<< HEAD

# --- Build Configuration ---
# --- Build Configuration ---
option(PROX_STATIC_BUILD "Build as a single static executable" ON)

if (PROX_STATIC_BUILD)
    add_definitions(-DPROX_STATIC)
    
    # Configure Static CRT for MSVC to avoid DLL dependencies
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        add_compile_options($<$<CONFIG:Release>:/MT> $<$<CONFIG:Debug>:/MTd>)
    endif()

    # Monolithic build: Sources directly in executable to ensure all symbols are included
    add_executable(proxpl src/main.c ${LIB_SOURCES})
else()
    # Traditional split build
    add_library(proxpl_lib SHARED ${LIB_SOURCES})
    if (LLVM_FOUND)
        target_link_libraries(proxpl_lib PRIVATE ${llvm_libs})
    endif()
    if (LIBFFI_FOUND)
        target_link_libraries(proxpl_lib PRIVATE ${LIBFFI_LIBRARIES})
    endif()
    add_executable(proxpl src/main.c)
    target_link_libraries(proxpl PRIVATE proxpl_lib)
endif()

# --- Main Executable Linking ---
# (Common linking logic)
if (LLVM_FOUND)
    target_link_libraries(proxpl PRIVATE ${llvm_libs})
endif()

if (LIBFFI_FOUND)
    target_link_libraries(proxpl PRIVATE ${LIBFFI_LIBRARIES})
endif()
=======
# --- Core Library ---
add_library(proxpl_lib SHARED ${LIB_SOURCES})
if (LLVM_FOUND)
    target_link_libraries(proxpl_lib PRIVATE ${llvm_libs})
endif()

# --- Main Executable ---
add_executable(proxpl src/main.c)
target_link_libraries(proxpl PRIVATE proxpl_lib ${llvm_libs})
>>>>>>> fix-ci-build

if(UNIX)
    if(APPLE)
        target_link_libraries(proxpl PRIVATE pthread dl z)
<<<<<<< HEAD
    else()
        target_link_libraries(proxpl PRIVATE pthread dl z tinfo)
=======
        target_link_libraries(proxpl_lib PRIVATE pthread dl z)
    else()
        target_link_libraries(proxpl PRIVATE pthread dl z tinfo)
        target_link_libraries(proxpl_lib PRIVATE pthread dl z tinfo)
>>>>>>> fix-ci-build
    endif()
endif()

# --- IR Test Executable ---
add_executable(ir_gen_test tools/ir_gen_test.c)
<<<<<<< HEAD
if(PROX_STATIC_BUILD)
    # Link directly to sources for static build since proxpl_lib doesn't exist
    target_sources(ir_gen_test PRIVATE ${LIB_SOURCES})
    if (LLVM_FOUND)
         target_link_libraries(ir_gen_test PRIVATE ${llvm_libs})
    endif()
    if (LIBFFI_FOUND)
         target_link_libraries(ir_gen_test PRIVATE ${LIBFFI_LIBRARIES})
    endif()
else()
    target_link_libraries(ir_gen_test PRIVATE proxpl_lib)
    if (LLVM_FOUND)
        target_link_libraries(ir_gen_test PRIVATE ${llvm_libs})
    endif()
    if (LIBFFI_FOUND)
        target_link_libraries(ir_gen_test PRIVATE ${LIBFFI_LIBRARIES})
    endif()
endif()
=======
target_link_libraries(ir_gen_test PRIVATE proxpl_lib)
>>>>>>> fix-ci-build

# --- LLVM Gen Test Executable ---
add_executable(llvm_gen_test tools/llvm_gen_test.c)

<<<<<<< HEAD
if(PROX_STATIC_BUILD)
    target_sources(llvm_gen_test PRIVATE ${LIB_SOURCES})
    if (LLVM_FOUND)
        target_link_libraries(llvm_gen_test PRIVATE ${llvm_libs})
    endif()
    if (LIBFFI_FOUND)
        target_link_libraries(llvm_gen_test PRIVATE ${LIBFFI_LIBRARIES})
    endif()
else()
    target_link_libraries(llvm_gen_test PRIVATE proxpl_lib ${llvm_libs})
    if (LIBFFI_FOUND)
        target_link_libraries(llvm_gen_test PRIVATE ${LIBFFI_LIBRARIES})
    endif()
endif()
=======
target_link_libraries(llvm_gen_test PRIVATE proxpl_lib ${llvm_libs})
>>>>>>> fix-ci-build

# --- PRM Executable ---
add_executable(prm 
    tools/prm_main.c 
    src/prm/manifest.c 
    src/prm/builder.c
<<<<<<< HEAD
    src/prm/commands/cmd_core.c
=======
>>>>>>> fix-ci-build
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
