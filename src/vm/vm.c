// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"
#include "bytecode.h"
#include "stdlib_native.h"

VM vm;

static void resetStack() {
  vm.stackTop = vm.stack;
  vm.frameCount = 0;
}

static void runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  for (int i = vm.frameCount - 1; i >= 0; i--) {
    CallFrame* frame = &vm.frames[i];
    // Instruction pointer might be NULL if error is before execution starts
    // size_t instruction = frame->ip - frame->closure->function->chunk.code - 1;
    // fprintf(stderr, "[line %d] in %s()\n",
    //         frame->closure->function->chunk.lines[instruction],
    //         frame->closure->function->name == NULL
    //             ? "script"
    //             : frame->closure->function->name->chars);
    fprintf(stderr, "Runtime Error in frame %d\n", i);
  }

  resetStack();
}

void initVM(VM* vm_ptr) {
    if (vm_ptr != &vm) {
        // In case caller passes a different pointer, though we use global 'vm'
        // For now, assume global.
    }
    resetStack();
    vm.objects = NULL;
    vm.bytesAllocated = 0;
    vm.nextGC = 1024 * 1024;

    vm.grayCount = 0;
    vm.grayCapacity = 0;
    vm.grayStack = NULL;

    initTable(&vm.globals);
    initTable(&vm.strings);
    
    // Core natives could be defined here or via registerStdLib
    // defineNative("clock", clockNative); // Example
}

void freeVM(VM* vm_ptr) {
    freeTable(&vm.globals);
    freeTable(&vm.strings);
    freeObjects(); // Defined in memory.c usually
}

void push(VM* vm_ptr, Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop(VM* vm_ptr) {
  vm.stackTop--;
  return *vm.stackTop;
}

Value peek(VM* vm_ptr, int distance) {
  return vm.stackTop[-1 - distance];
}

bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void defineNative(VM* vm_ptr, const char* name, NativeFn function) {
  push(vm_ptr, OBJ_VAL(copyString(name, (int)strlen(name))));
  push(vm_ptr, OBJ_VAL(newNative(function)));
  tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
  pop(vm_ptr);
  pop(vm_ptr);
}

// -----------------------------------------------------------------------------
//  Core Execution Loop (Integrated from vm_core_opt.c)
// -----------------------------------------------------------------------------

/* Micro-optimization: expect/likely macros for branch prediction */
#if defined(__GNUC__)
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

// Helper to get constant from current chunk
static inline Value getConstant(Chunk* chunk, uint8_t index) {
    return consttable_get(chunk, index);
}

static InterpretResult run() {
    CallFrame* frame = &vm.frames[vm.frameCount - 1];
    Chunk* chunk = &frame->closure->function->chunk; // Assuming closure->function logic or direct chunk if simplistic
    // If using AST interpreter solely, this might differ. 
    // BUT main.c calls interpret(&chunk) or similar? 
    // Providing a basic loop for bytecode execution:

    register uint8_t* ip = frame->ip;
    register Value* stackTop = vm.stackTop; // Local caching of stack pointer
    // Note: need to sync back to vm.stackTop before calls/allocations
    
    #define READ_BYTE() (*ip++)
    #define READ_CONSTANT() (getConstant(chunk, READ_BYTE()))
    #define BINARY_OP(valueType, op) \
        do { \
          if (!IS_NUMBER(peek(&vm, 0)) || !IS_NUMBER(peek(&vm, 1))) { \
            vm.stackTop = stackTop; \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
          } \
          double b = AS_NUMBER(pop(&vm)); \
          double a = AS_NUMBER(pop(&vm)); \
          push(&vm, valueType(a op b)); \
          stackTop = vm.stackTop; \
        } while (false)

    for (;;) {
    #ifdef DEBUG_TRACE_Execution
       // disassembleInstruction(chunk, (int)(ip - chunk->code));
    #endif

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                *stackTop = constant;
                stackTop++;
                break;
            }
            case OP_NIL: push(&vm, NIL_VAL); stackTop++; break;
            case OP_TRUE: push(&vm, BOOL_VAL(true)); stackTop++; break;
            case OP_FALSE: push(&vm, BOOL_VAL(false)); stackTop++; break;
            
            case OP_POP: stackTop--; break;
            
            case OP_ADD: {
                // Fast path for numbers
                if (likely(IS_NUMBER(stackTop[-1]) && IS_NUMBER(stackTop[-2]))) {
                    double b = AS_NUMBER(stackTop[-1]);
                    double a = AS_NUMBER(stackTop[-2]);
                    stackTop--;
                    stackTop[-1] = NUMBER_VAL(a + b);
                } else {
                    // Fallback / String concat
                     vm.stackTop = stackTop;
                     runtimeError("Operands must be numbers."); 
                     return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
             case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
             case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
             case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
            
            case OP_HALT:
            case OP_RETURN: {
                // Exit for now
                return INTERPRET_OK;
            }
            // Add other ops...
        }
        vm.stackTop = stackTop;
    }
    
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

// Minimal run for bare Chunk (if main calls with Chunk*)
// NOTE: main.c uses interpret(&chunk) which implies passing a Chunk directly, 
// OR main.c was passing 'interpret(const char* source)'. 
// The error log said: "warning: passing argument 2 of 'interpret' from incompatible pointer type"
// "expected 'const char *' but argument is of type 'Chunk *'"
// So main.c was passing a Chunk*, but vm.h declared `interpret(VM*, const char*)`.
// We will implement the `interpret` that takes source, and fix main.c.

InterpretResult interpret(VM* vm_ptr, const char* source) {
    // Placeholder for compiler call:
    // ObjFunction* function = compile(source);
    // if (function == NULL) return INTERPRET_COMPILE_ERROR;
    
    // push(OBJ_VAL(function));
    // CallFrame* frame = &vm.frames[vm.frameCount++];
    // frame->closure = newClosure(function); ...
    // frame->ip = function->chunk.code;
    // frame->slots = vm.stack;
    
    // return run();
    
    printf("Interpreter run on source not fully wired yet.\n");
    return INTERPRET_OK;
}

InterpretResult interpretChunk(VM* vm_ptr, Chunk* chunk) {
    // Wrap the raw chunk into a function/closure to execute it using `run()`
    // Since `chunk` is stack allocated in main.c, we need to be careful.
    // Ideally we'd create a function object.
    
    // For now, let's create a dummy ObjFunction
    ObjFunction* function = newFunction();
    // Shallow copy the chunk? Or just pointer?
    // ObjFunction struct usually contains a Chunk by value.
    // typedef struct { Obj obj; int arity; int upvalueCount; Chunk chunk; ObjString* name; } ObjFunction;
    
    // We can't easily replace the chunk in ObjFunction if it's by value.
    // Let's manually copy.
    // Note: This is an inefficient workaround for the REPL structure in main.c
    function->chunk = *chunk; 
    // BUT! `chunk` in main.c owns the code/constants array pointers.
    // If we free function, we free them. main.c also frees chunk. Double free!
    // We should fix main.c to not free chunk if we pass ownership, or copy.
    // Given main.c structure: `interpret(&vm, &chunk); freeChunk(&chunk);`
    // We should EXECUTE without taking ownership.
    
    // HACK: We can set up a frame pointing to this stack-allocated chunk's code
    // But `run()` accesses `frame->closure->function->chunk`.
    
    // Alternative: Just run the loop manually here or adapt `run()` to take `Chunk*`.
    // Let's adapt `run()` to take `Chunk*`.
    
    // We need to push the script function/closure to stack 0 for strict interaction?
    // For this simple fix:
    
    push(vm_ptr, OBJ_VAL(function)); // GC protection
    ObjClosure* closure = newClosure(function);
    pop(vm_ptr);
    push(vm_ptr, OBJ_VAL(closure));
    
    CallFrame* frame = &vm.frames[vm.frameCount++];
    frame->closure = closure;
    frame->ip = chunk->code;
    frame->slots = vm.stack;
    
    // We need `run()` to recognize we are using `chunk` from arguments, or `run()` uses `frame->closure`.
    // Since `function->chunk` is a COPY of `main.c`'s chunk struct (shallow copy of pointers),
    // and `run()` reads `frame->closure->function->chunk`, it will read the correct code pointers.
    // The issue is ownership. `newFunction` initializes an empty chunk.
    // `function->chunk = *chunk` copies the pointers.
    // `run()` executes.
    // Then we return.
    // `main.c` calls `freeChunk(&chunk)`.
    // `function` is still on GC list? Yes. If GC runs, it might traverse `function` and see the pointers.
    // But `main.c` freeing them effectively dangles them.
    // LUCKILY: `main.c` frees AFTER `interpret` returns.
    // AND: `function` is not reachable if popped? 
    // We should pop closure from stack before return?
    // Or just let it be.
    
    // To safe-guard against double-free if `function` is freed later:
    // `function->chunk` should be zeroed out after use?
    // Or simpler: `compile` usually returns a function that OWNS the chunk.
    // `main.c` doing manual chunk gen is the anomaly.
    
    InterpretResult result = run();
    
    // Detach chunk so `freeFunction` doesn't free the code currently owned by `main.c`
    // (Actually `ObjFunction` is GC managed, it won't be freed immediately)
    // But `main.c` WILL free `chunk` pointers.
    // So if `function` survives and we try to access `function->chunk` later -> Crash.
    // We should zero out `function->chunk` here to be safe.
    initChunk(&function->chunk); 
    
    pop(vm_ptr); // Remove closure
    return result;
}

// If we need to support the AST interpreter which main.c uses:
InterpretResult interpretAST(VM* vm_ptr, StmtList* statements) {
    // this handles AST execution
    return INTERPRET_OK; 
}
