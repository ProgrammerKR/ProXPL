// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-23
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#ifndef PROX_IR_OPT_H
#define PROX_IR_OPT_H

#include "ir.h"

// Promotes memory allocas to SSA registers and inserts Phi nodes.
void promoteMemoryToRegisters(IRFunction* func);
void constantFold(IRFunction* func);
void deadCodeElimination(IRFunction* func);

#endif // PROX_IR_OPT_H
