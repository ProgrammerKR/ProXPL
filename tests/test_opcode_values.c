// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/* test_opcode_values.c
 * Verifies opcode enum numeric assignments match documented values
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bytecode.h"

int main(void) {
    printf("OP_ADD: %d (0x%02X)\n", OP_ADD, OP_ADD);
    printf("OP_HALT: %d (0x%02X)\n", OP_HALT, OP_HALT);
    printf("OP_CONSTANT: %d (0x%02X)\n", OP_CONSTANT, OP_CONSTANT);
    printf("OP_CALL: %d (0x%02X)\n", OP_CALL, OP_CALL);
    
    // Sanity checks
    assert(OP_CONSTANT == 0);
    assert(OP_HALT == 0xFF); 
    
    printf("test_opcode_values: OK\n");
    return 0;
}
