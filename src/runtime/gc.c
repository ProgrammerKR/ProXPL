#include <stdlib.h>
#include <stdio.h>
#include "../include/gc.h"
#include "../include/object.h"
#include "../include/compiler.h"
#include "../include/table.h"
#include "../include/memory.h"
#include "../include/vm.h"

#ifdef DEBUG_LOG_GC
#include "../include/debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

// Access the global VM instance
extern VM vm;

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    vm.bytesAllocated += newSize - oldSize;
    
    if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
        collectGarbage(&vm);
#endif
        if (vm.bytesAllocated > vm.nextGC) {
            collectGarbage(&vm);
        }
    }

    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}

void markObject(Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;

#ifdef DEBUG_LOG_GC
    printf("%p mark ", (void*)object);
    printObject(OBJ_VAL(object));
    printf("\n");
#endif

    object->isMarked = true;

    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        vm.grayStack = (Obj**)realloc(vm.grayStack, sizeof(Obj*) * vm.grayCapacity);
        // If allocation fails here, we are in trouble. For a toy GC, exit(1) is "fine".
        if (vm.grayStack == NULL) exit(1);
    }
    
    vm.grayStack[vm.grayCount++] = object;
}

void markValue(Value value) {
    if (IS_OBJ(value)) markObject(AS_OBJ(value));
}

static void markArray(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        markValue(array->values[i]);
    }
}

static void blackenObject(Obj* object) {
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void*)object);
    printObject(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_NATIVE:
        case OBJ_STRING:
            break;
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            markObject((Obj*)function->name);
            markArray(&function->chunk.constants);
            break;
        }
        // Case OBJ_CLASS, OBJ_INSTANCE would go here
        default:
            // Warn or ignore?
            break;
    }
}

static void markRoots() {
    // 1. Mark Stack
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }

    // 2. Mark Globals
    markTable(&vm.globals);
    
    // 3. Mark Compiler Roots
    markCompilerRoots();
    
    // 4. Mark Interned Strings?
    // ProXPL interns all strings. Logic: If we mark weak references (interned strings), 
    // we keep them alive.
    // Usually languages use a weak table for interning. 
    // If we markTable(&vm.strings), we never free strings until VM shutdown.
    // For now, let's NOT mark strings table effectively acting as weak ref clearing
    // but ProXPL `table.c` implementation is a strong map.
    // If we don't mark `vm.strings`, dependent strings might vanish and `vm.strings` 
    // will hold dangling pointers?
    // Correct approach for simple intern table:
    // Mark it as a root? Yes, keeps all strings forever.
    // "removeWhite" from table (weak semantics)?
    // `tableRemoveWhite` exists in generic table.c/h! 
    // So we do NOT mark vm.strings here. We treat it specially in sweep phase.
}

static void traceReferences() {
    while (vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        blackenObject(object);
    }
}

static void freeObject(Obj* object) {
#ifdef DEBUG_LOG_GC
    printf("%p free ", (void*)object);
    printObject(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            // Flexible array member 'chars' is part of struct allocation
            // so just freeing object is enough?
            // Wait, allocateString does: sizeof(ObjString) + length + 1
            // So yes, single free.
            FREE(ObjString, object);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            freeChunk(&function->chunk);
            FREE(ObjFunction, object);
            break;
        }
        case OBJ_NATIVE: {
            FREE(ObjNative, object);
            break;
        }
        default:
            FREE(Obj, object); // Fallback
            break;
    }
}

static void sweep() {
    Obj* previous = NULL;
    Obj* object = vm.objects;
    
    while (object != NULL) {
        if (object->isMarked) {
            object->isMarked = false; // Unmark for next cycle
            previous = object;
            object = object->next;
        } else {
            Obj* unreached = object;
            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                vm.objects = object;
            }
            
            freeObject(unreached);
        }
    }
}

void collectGarbage(VM* vm_ptr) {
    if (vm_ptr != &vm) { 
        // Logic error or multi-vm support? 
        // For now assume single global VM
    }

#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = vm.bytesAllocated;
#endif

    markRoots();
    traceReferences();
    
    // Weak ref handling for interned strings
    tableRemoveWhite(&vm.strings);
    
    sweep();
    
    vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
    printf("   collected %zu bytes (from %zu to %zu) next at %zu\n",
           before - vm.bytesAllocated, before, vm.bytesAllocated, vm.nextGC);
#endif
}

void freeObjects(VM* vm_ptr) {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
    
    free(vm.grayStack);
    vm.grayStack = NULL;
}
