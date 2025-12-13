cmake_minimum_required(VERSION 3.10)


add_executable(bytecode_tests bytecode_tests.c)
target_link_libraries(bytecode_tests PRIVATE prox_core)
target_include_directories(bytecode_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME BytecodeTests COMMAND bytecode_tests)

add_executable(test_opcode_roundtrip test_opcode_roundtrip.c)
target_link_libraries(test_opcode_roundtrip PRIVATE prox_core)
target_include_directories(test_opcode_roundtrip PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME OpcodeRoundTrip COMMAND test_opcode_roundtrip)

add_executable(test_opcode_values test_opcode_values.c)
target_link_libraries(test_opcode_values PRIVATE prox_core)
target_include_directories(test_opcode_values PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME OpcodeValues COMMAND test_opcode_values)
