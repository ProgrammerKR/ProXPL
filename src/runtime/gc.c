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

void initGC(VM* vm) {
    vm->grayCount = 0;
    vm->grayCapacity = 0;
    vm->grayStack = NULL;
    vm->bytesAllocated = 0;
    vm->nextGC = 1024 * 1024;
}

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
        case OBJ_MODULE: {
            ObjModule* module = (ObjModule*)object;
            markObject((Obj*)module->name);
            markTable(&module->exports);
            break;
        }
        case OBJ_FOREIGN: {
            ObjForeign* foreign = (ObjForeign*)object;
            markObject((Obj*)foreign->name);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            markObject((Obj*)closure->function);
            for (int i = 0; i < closure->upvalueCount; i++) {
                markObject((Obj*)closure->upvalues[i]);
            }
            break;
        }
        case OBJ_UPVALUE:
            markValue(((ObjUpvalue*)object)->closed);
            break;
        case OBJ_CLASS: {
            struct ObjClass* klass = (struct ObjClass*)object;
            markObject((Obj*)klass->name);
            markTable(&klass->methods);
            break;
        }
        case OBJ_INSTANCE: {
            struct ObjInstance* instance = (struct ObjInstance*)object;
            markObject((Obj*)instance->klass);
            markTable(&instance->fields);
            break;
        }
        case OBJ_BOUND_METHOD: {
            struct ObjBoundMethod* bound = (struct ObjBoundMethod*)object;
            markValue(bound->receiver);
            markObject((Obj*)bound->method);
            break;
        }
        case OBJ_LIST: {
            struct ObjList* list = (struct ObjList*)object;
            for (int i = 0; i < list->count; i++) {
                markValue(list->items[i]);
            }
            break;
        }
        case OBJ_DICTIONARY: {
            struct ObjDictionary* dict = (struct ObjDictionary*)object;
            markTable(&dict->items);
            break;
        }
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

    // 2. Mark Frames (Function Closures)
    for (int i = 0; i < vm.frameCount; i++) {
        markObject((Obj*)vm.frames[i].closure);
    }

    // 3. Mark Open Upvalues
    for (ObjUpvalue* upvalue = vm.openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        markObject((Obj*)upvalue);
    }

    // 2. Mark Globals
    markTable(&vm.globals);
    
    // 3. Mark Loaded Modules & Search Paths?
    // Modules are strong refs in importer.modules
    markTable(&vm.importer.modules);

    // 4. Mark Compiler Roots
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
        case OBJ_FOREIGN: {
            // Note: We do not close the library handle here as it might be shared.
            // Future improvement: Reference counting for libraries.
            FREE(ObjForeign, object);
            break;
        }
        case OBJ_MODULE: {
            ObjModule* module = (ObjModule*)object;
            freeTable(&module->exports);
            FREE(ObjModule, object);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
            FREE(ObjClosure, object);
            break;
        }
        case OBJ_UPVALUE:
            FREE(ObjUpvalue, object);
            break;
        case OBJ_CLASS: {
            struct ObjClass* klass = (struct ObjClass*)object;
            freeTable(&klass->methods);
            FREE(struct ObjClass, object);
            break;
        }
        case OBJ_INSTANCE: {
            struct ObjInstance* instance = (struct ObjInstance*)object;
            freeTable(&instance->fields);
            FREE(struct ObjInstance, object);
            break;
        }
        case OBJ_BOUND_METHOD:
            FREE(struct ObjBoundMethod, object);
            break;
        case OBJ_LIST: {
            struct ObjList* list = (struct ObjList*)object;
            FREE_ARRAY(Value, list->items, list->capacity);
            FREE(struct ObjList, object);
            break;
        }
        case OBJ_DICTIONARY: {
            struct ObjDictionary* dict = (struct ObjDictionary*)object;
            freeTable(&dict->items);
            FREE(struct ObjDictionary, object);
            break;
        }
        case OBJ_TASK: {
            // coroHandle is void* managed by LLVM/malloc?
            // If we used a custom allocator, we free it here.
            // But LLVM `coro.destroy` handles it.
            // We should call `llvm.coro.destroy(hdl)` if task is dead?
            // But we can't easily call LLVM intrinsic from here.
            // We assume LLVM code cleaned up via `coro.free` path 
            // when task reached end.
            // If task is collected BEFORE completion, we might leak the coroutine frame.
            // For now, simple free of ObjTask struct.
            FREE(struct ObjTask, object);
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
    (void)vm_ptr;
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
    
    free(vm.grayStack);
    vm.grayStack = NULL;
}
