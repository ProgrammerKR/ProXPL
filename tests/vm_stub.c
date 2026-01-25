#include "../include/vm.h"
#include <stdio.h>

VM vm;

void push(VM* vm, Value value) {
    // Stub: do nothing or simple stack increment
    if (vm->stackTop) {
        *vm->stackTop = value;
        vm->stackTop++;
    }
}

Value pop(VM* vm) {
    // Stub
    if (vm->stackTop > vm->stack) {
        vm->stackTop--;
        return *vm->stackTop;
    }
    return NIL_VAL;
}

Value peek(VM* vm, int distance) {
    return vm->stackTop[-1 - distance];
}

void markCompilerRoots() {
    // Stub
}

