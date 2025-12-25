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
    assert(OP_ADD == 0x01);
    assert(OP_HALT == 0xFF);
    assert(OP_PUSH_CONST == 0x30);
    assert(OP_CALL == 0x50);
    printf("test_opcode_values: OK\n");
    return 0;
}
