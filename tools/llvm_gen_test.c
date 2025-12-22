#include <stdio.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/ir.h"
#include "../include/ir_opt.h"
#include "../include/backend_llvm.h"
#include "../include/vm.h"

VM vm;

int main() {
    initVM();
    
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
    
    Parser parser;
    initParser(&parser, source);
    StmtList* program = parse(&parser);
    
    if (program) {
        IRModule* ir = generateSSA_IR(program);
        
        for (int i = 0; i < ir->funcCount; i++) {
            promoteMemoryToRegisters(ir->functions[i]);
            constantFold(ir->functions[i]);
            deadCodeElimination(ir->functions[i]);
        }
        
        printf("Generated LLVM IR:\n");
        emitLLVM(ir);
        
        freeIRModule(ir);
        // Free program, etc.
    }
    
    freeVM();
    return 0;
}
