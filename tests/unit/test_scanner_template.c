// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * test_scanner.c
 * Unit tests for the lexical analyzer (scanner/tokenizer)
 * 
 * This file demonstrates the test structure using Unity testing framework.
 * Compile and run with: gcc -I../include test_scanner.c ../src/lexer/scanner.c -o test_scanner && ./test_scanner
 */

#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "../include/scanner.h"

// Test fixtures
static Scanner scanner;

/*
 * setUp
 * Runs before each test
 */
void setUp(void) {
    // Initialize scanner for testing
    // Reset any global state
}

/*
 * tearDown
 * Runs after each test
 */
void tearDown(void) {
    // Clean up resources
}

/* ============================================
 * BASIC TOKENIZATION TESTS
 * ============================================ */

/*
 * Test: Scanning simple identifier
 */
void test_scanner_identifier(void) {
    const char *source = "hello";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
    TEST_ASSERT_EQUAL_STRING("hello", token.start);
    TEST_ASSERT_EQUAL_INT(5, token.length);
}

/*
 * Test: Scanning underscore-prefixed identifier
 */
void test_scanner_identifier_underscore(void) {
    const char *source = "_private";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
    TEST_ASSERT_EQUAL_INT(8, token.length);
}

/*
 * Test: Scanning identifier with numbers
 */
void test_scanner_identifier_with_numbers(void) {
    const char *source = "var2d";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
    TEST_ASSERT_EQUAL_INT(5, token.length);
}

/* ============================================
 * NUMBER TOKENIZATION TESTS
 * ============================================ */

/*
 * Test: Scanning integer literal
 */
void test_scanner_integer_literal(void) {
    const char *source = "42";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, token.type);
    TEST_ASSERT_EQUAL_STRING("42", token.start);
    TEST_ASSERT_EQUAL_INT(2, token.length);
}

/*
 * Test: Scanning floating-point literal
 */
void test_scanner_float_literal(void) {
    const char *source = "3.14159";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, token.type);
    TEST_ASSERT_EQUAL_INT(7, token.length);
}

/*
 * Test: Scanning scientific notation
 */
void test_scanner_scientific_notation(void) {
    const char *source = "1.5e-10";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, token.type);
}

/*
 * Test: Scanning zero
 */
void test_scanner_zero(void) {
    const char *source = "0";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, token.type);
    TEST_ASSERT_EQUAL_INT(1, token.length);
}

/* ============================================
 * STRING TOKENIZATION TESTS
 * ============================================ */

/*
 * Test: Scanning simple string
 */
void test_scanner_string_simple(void) {
    const char *source = "\"hello\"";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_STRING, token.type);
    TEST_ASSERT_EQUAL_INT(7, token.length);
}

/*
 * Test: Scanning empty string
 */
void test_scanner_string_empty(void) {
    const char *source = "\"\"";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_STRING, token.type);
}

/*
 * Test: Scanning string with escape sequences
 */
void test_scanner_string_with_escapes(void) {
    const char *source = "\"hello\\nworld\"";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_STRING, token.type);
}

/* ============================================
 * KEYWORD TOKENIZATION TESTS
 * ============================================ */

/*
 * Test: Scanning 'let' keyword
 */
void test_scanner_keyword_let(void) {
    const char *source = "let";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_LET, token.type);
}

/*
 * Test: Scanning 'func' keyword
 */
void test_scanner_keyword_func(void) {
    const char *source = "func";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_FUNC, token.type);
}

/*
 * Test: Scanning 'true' keyword
 */
void test_scanner_keyword_true(void) {
    const char *source = "true";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_TRUE, token.type);
}

/*
 * Test: Distinguish keyword from identifier
 */
void test_scanner_keyword_vs_identifier(void) {
    const char *source = "letme";  // Not the keyword 'let'
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
    TEST_ASSERT_EQUAL_STRING("letme", token.start);
}

/* ============================================
 * OPERATOR TOKENIZATION TESTS
 * ============================================ */

/*
 * Test: Scanning plus operator
 */
void test_scanner_operator_plus(void) {
    const char *source = "+";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_PLUS, token.type);
}

/*
 * Test: Scanning equals operator
 */
void test_scanner_operator_equal(void) {
    const char *source = "=";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_EQUAL, token.type);
}

/*
 * Test: Scanning double-character operator (==)
 */
void test_scanner_operator_equal_equal(void) {
    const char *source = "==";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_EQUAL_EQUAL, token.type);
    TEST_ASSERT_EQUAL_INT(2, token.length);
}

/*
 * Test: Scanning arrow operator
 */
void test_scanner_operator_arrow(void) {
    const char *source = "=>";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_ARROW, token.type);
}

/* ============================================
 * PUNCTUATION TESTS
 * ============================================ */

/*
 * Test: Scanning left parenthesis
 */
void test_scanner_lparen(void) {
    const char *source = "(";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_LEFT_PAREN, token.type);
}

/*
 * Test: Scanning semicolon
 */
void test_scanner_semicolon(void) {
    const char *source = ";";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_SEMICOLON, token.type);
}

/*
 * Test: Scanning comma
 */
void test_scanner_comma(void) {
    const char *source = ",";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_COMMA, token.type);
}

/* ============================================
 * COMMENT TESTS
 * ============================================ */

/*
 * Test: Single-line comment is skipped
 */
void test_scanner_comment_single_line(void) {
    const char *source = "// Comment\n42";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    // Should skip comment and return the number
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, token.type);
    TEST_ASSERT_EQUAL_STRING("42", token.start);
}

/*
 * Test: Multi-line comment is skipped
 */
void test_scanner_comment_multi_line(void) {
    const char *source = "/* Comment */42";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, token.type);
}

/* ============================================
 * SEQUENCE TESTS
 * ============================================ */

/*
 * Test: Scanning multiple tokens in sequence
 */
void test_scanner_token_sequence(void) {
    const char *source = "let x = 5;";
    initScanner(&scanner, source);
    
    Token t1 = scanner_next_token(&scanner);
    Token t2 = scanner_next_token(&scanner);
    Token t3 = scanner_next_token(&scanner);
    Token t4 = scanner_next_token(&scanner);
    Token t5 = scanner_next_token(&scanner);
    Token t6 = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_LET, t1.type);
    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, t2.type);
    TEST_ASSERT_EQUAL(TOKEN_EQUAL, t3.type);
    TEST_ASSERT_EQUAL(TOKEN_NUMBER, t4.type);
    TEST_ASSERT_EQUAL(TOKEN_SEMICOLON, t5.type);
    TEST_ASSERT_EQUAL(TOKEN_EOF, t6.type);
}

/* ============================================
 * LINE AND COLUMN TRACKING TESTS
 * ============================================ */

/*
 * Test: Line number tracking
 */
void test_scanner_line_tracking(void) {
    const char *source = "x\ny\nz";
    initScanner(&scanner, source);
    
    Token t1 = scanner_next_token(&scanner);
    TEST_ASSERT_EQUAL_INT(1, t1.line);
    
    Token t2 = scanner_next_token(&scanner);
    TEST_ASSERT_EQUAL_INT(2, t2.line);
    
    Token t3 = scanner_next_token(&scanner);
    TEST_ASSERT_EQUAL_INT(3, t3.line);
}

/* ============================================
 * ERROR CASES
 * ============================================ */

/*
 * Test: Unterminated string
 */
void test_scanner_error_unterminated_string(void) {
    const char *source = "\"unterminated";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_ERROR, token.type);
}

/*
 * Test: Unknown character
 */
void test_scanner_error_unknown_character(void) {
    const char *source = "@";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_ERROR, token.type);
}

/* ============================================
 * EOF TESTS
 * ============================================ */

/*
 * Test: End of file detection
 */
void test_scanner_eof(void) {
    const char *source = "x";
    initScanner(&scanner, source);
    
    scanner_next_token(&scanner);  // Consume 'x'
    Token eof = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_EOF, eof.type);
}

/*
 * Test: EOF on empty source
 */
void test_scanner_empty_source(void) {
    const char *source = "";
    initScanner(&scanner, source);
    
    Token token = scanner_next_token(&scanner);
    
    TEST_ASSERT_EQUAL(TOKEN_EOF, token.type);
}

/* ============================================
 * MAIN TEST RUNNER
 * ============================================ */

int main(void) {
    UNITY_BEGIN();
    
    // Basic tokenization
    RUN_TEST(test_scanner_identifier);
    RUN_TEST(test_scanner_identifier_underscore);
    RUN_TEST(test_scanner_identifier_with_numbers);
    
    // Numbers
    RUN_TEST(test_scanner_integer_literal);
    RUN_TEST(test_scanner_float_literal);
    RUN_TEST(test_scanner_scientific_notation);
    RUN_TEST(test_scanner_zero);
    
    // Strings
    RUN_TEST(test_scanner_string_simple);
    RUN_TEST(test_scanner_string_empty);
    RUN_TEST(test_scanner_string_with_escapes);
    
    // Keywords
    RUN_TEST(test_scanner_keyword_let);
    RUN_TEST(test_scanner_keyword_func);
    RUN_TEST(test_scanner_keyword_true);
    RUN_TEST(test_scanner_keyword_vs_identifier);
    
    // Operators
    RUN_TEST(test_scanner_operator_plus);
    RUN_TEST(test_scanner_operator_equal);
    RUN_TEST(test_scanner_operator_equal_equal);
    RUN_TEST(test_scanner_operator_arrow);
    
    // Punctuation
    RUN_TEST(test_scanner_lparen);
    RUN_TEST(test_scanner_semicolon);
    RUN_TEST(test_scanner_comma);
    
    // Comments
    RUN_TEST(test_scanner_comment_single_line);
    RUN_TEST(test_scanner_comment_multi_line);
    
    // Sequences
    RUN_TEST(test_scanner_token_sequence);
    
    // Line tracking
    RUN_TEST(test_scanner_line_tracking);
    
    // Errors
    RUN_TEST(test_scanner_error_unterminated_string);
    RUN_TEST(test_scanner_error_unknown_character);
    
    // EOF
    RUN_TEST(test_scanner_eof);
    RUN_TEST(test_scanner_empty_source);
    
    return UNITY_END();
}
