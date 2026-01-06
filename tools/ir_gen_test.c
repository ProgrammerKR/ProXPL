// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "../include/common.h"
#include "../include/scanner.h"
#include "../include/parser.h"
#include "../include/ir.h"
#include "../include/ir_opt.h"
#include "../include/vm.h"
#include <stdio.h>
#include <stdlib.h>



// Forward declaration from ir_gen.c
IRModule* generateSSA_IR(StmtList* program);

void test_ir(const char* source) {
    printf("Testing Source:\n%s\n", source);

    Scanner scanner;
    initScanner(&scanner, source);

    Token tokens[4096];
    int tokenCount = 0;

    for (;;) {
        Token token = scanToken(&scanner);
        tokens[tokenCount++] = token;
        if (token.type == TOKEN_EOF || token.type == TOKEN_ERROR) break;
    }

    Parser parser;
    initParser(&parser, tokens, tokenCount, source);
    StmtList* statements = parse(&parser);

    if (!statements) {
        printf("Parse failed\n");
        return;
    }

    IRModule* ir = generateSSA_IR(statements);
    
    // Run optimizations
    for (int i = 0; i < ir->funcCount; i++) {
        promoteMemoryToRegisters(ir->functions[i]);
        constantFold(ir->functions[i]);
        deadCodeElimination(ir->functions[i]);
    }

    printf("\nGenerated Optimized IR:\n");
    dumpIR(ir);

    freeIRModule(ir);
    freeStmtList(statements);
}

int main() {
    initVM(&vm); // Initialize VM for string interning and GC

    const char* code = 
        "let x = 10;\n"
        "if (x > 5) {\n"
        "    x = x - 1;\n"
        "} else {\n"
        "    x = x + 1;\n"
        "}\n"
        "let z = 10 * 5;\n"
        "while (x > 0) {\n"
        "    x = x - 1;\n"
        "}\n"
        "return x + z;";
    
    test_ir(code);
    
    freeVM(&vm); // Cleanup
    return 0;
}
