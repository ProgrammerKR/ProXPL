#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/common.h"
#include "../include/scanner.h"
#include "../include/parser.h"
#include "../include/compiler.h"
#include "../include/backend_llvm.h"
#include "../include/vm.h"

VM vm;

int main() {
    initVM(&vm);
    
    const char* source = 
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
        "return x + z;\n";
    
    printf("Testing Source:\n%s\n", source);
    
    // 1. Tokenize
    Scanner scanner;
    initScanner(&scanner, source);
    Token tokens[4096];
    int tokenCount = 0;
    
    for (;;) {
        Token token = scanToken(&scanner);
        tokens[tokenCount++] = token;
        if (token.type == TOKEN_EOF) break;
        if (token.type == TOKEN_ERROR) {
            fprintf(stderr, "Scan Error: %.*s\n", token.length, token.start);
            return 1;
        }
    }
    
    // 2. Parse
    Parser parser;
    initParser(&parser, tokens, tokenCount);
    StmtList* program = parse(&parser);
    
    if (program) {
        printf("Parsed successfully.\n");
        
        IRModule* ir = generateSSA_IR(program);
        
        for (int i = 0; i < ir->funcCount; i++) {
            promoteMemoryToRegisters(ir->functions[i]);
            constantFold(ir->functions[i]);
            deadCodeElimination(ir->functions[i]);
        }
        
        printf("Generated LLVM IR:\n");
        // emitLLVM is defined in backend_llvm.cpp and exposed via extern "C"
        emitLLVM(ir);
        
        freeIRModule(ir);
        freeStmtList(program);
    } else {
        printf("Parse failed.\n");
    }
    
    freeVM(&vm);
    return 0;
}
