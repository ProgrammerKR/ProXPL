cmake_minimum_required(VERSION 3.15)

# ==============================================================================
# ProXPL Test Suite Configuration
# ==============================================================================

# --- C/C++ VM Unit Tests ---
set(VM_TEST_TARGETS)

macro(add_prox_c_test TARGET_NAME SOURCE_FILE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        add_executable(${TARGET_NAME} ${SOURCE_FILE})
        target_link_libraries(${TARGET_NAME} PRIVATE prox_core)
        target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
        add_test(NAME "Unit_${TARGET_NAME}" COMMAND ${TARGET_NAME})
        set_tests_properties("Unit_${TARGET_NAME}" PROPERTIES
            LABELS "unit;vm"
            TIMEOUT 15
        )
        list(APPEND VM_TEST_TARGETS ${TARGET_NAME})
    endif()
endmacro()

add_prox_c_test(bytecode_tests vm/bytecode_tests.c)
add_prox_c_test(test_opcode_roundtrip vm/test_opcode_roundtrip.c)
add_prox_c_test(test_opcode_values vm/test_opcode_values.c)

# --- Helper macro for registering .prox test suites ---
macro(add_prox_script_suite SUITE_NAME SUITE_DIR LABEL_NAME)
    file(GLOB_RECURSE SUITE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SUITE_DIR}/*.prox")
    foreach(TEST_FILE ${SUITE_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

        # Exclude specific tests with known platform-specific issues
        if(WIN32 AND TEST_NAME STREQUAL "v1_6_3_features")
            continue()
        endif()

        set(TEST_IDENTIFIER "${SUITE_NAME}_${TEST_NAME}")
        add_test(NAME "${TEST_IDENTIFIER}"
                 COMMAND proxpl "${TEST_FILE}")
        set_tests_properties("${TEST_IDENTIFIER}" PROPERTIES
            LABELS "${LABEL_NAME};script"
            TIMEOUT 10
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    endforeach()
endmacro()

# --- 1. Language Core Tests ---
add_prox_script_suite("Lang" "language" "language")

# --- 2. Integration & Standard Library Tests ---
add_prox_script_suite("Integ" "integration" "integration")

# --- 3. Intent-Oriented, Context-Aware & ASR Tests (IOP) ---
add_prox_script_suite("IOP" "iop" "iop")

# --- 4. Miscellaneous Feature Tests ---
add_prox_script_suite("Misc" "misc" "misc")

# --- 5. Regression Reproduction Tests ---
add_prox_script_suite("Repro" "repro" "repro")

# --- 6. Root-Level Script Tests ---
file(GLOB ROOT_PROX_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/*.prox")
foreach(TEST_FILE ${ROOT_PROX_TESTS})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    set(TEST_IDENTIFIER "Root_${TEST_NAME}")
    add_test(NAME "${TEST_IDENTIFIER}"
             COMMAND proxpl "${TEST_FILE}")
    set_tests_properties("${TEST_IDENTIFIER}" PROPERTIES
        LABELS "root;script"
        TIMEOUT 10
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endforeach()
