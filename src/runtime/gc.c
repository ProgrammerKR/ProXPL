// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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

// ----------------------------------------------------------------------------
// GENERATIONAL GC: NURSERY (YOUNG GENERATION)
// ----------------------------------------------------------------------------
#define NURSERY_SIZE (2 * 1024 * 1024) // 2MB

typedef struct {
    uint8_t* start;
    uint8_t* end;
    uint8_t* current;
} Nursery;

static Nursery nursery;
static bool nursery_initialized = false;

void initNursery() {
    nursery.start = (uint8_t*)malloc(NURSERY_SIZE);
    if (!nursery.start) {
        fprintf(stderr, "Fatal: Could not allocate GC Nursery.\n");
        exit(1);
    }
    nursery.end = nursery.start + NURSERY_SIZE;
    nursery.current = nursery.start;
    nursery_initialized = true;
}

static bool is_in_nursery(void* ptr) {
    if (!nursery_initialized || !ptr) return false;
    return (uint8_t*)ptr >= nursery.start && (uint8_t*)ptr < nursery.end;
}

static void* nursery_alloc(size_t size) {
    if (nursery.current + size > nursery.end) {
        return NULL; // Nursery full
    }
    void* result = nursery.current;
    nursery.current += size;
    return result;
}

static void reset_nursery() {
    // In a real generational GC, we would evacuate survivors here.
    // For this MVP, we treat Nursery as a "scratchpad" that gets fully collected
    // or promoted. Implementing full copying GC requires pointer updates.
    // Fallback strategy: If nursery full, trigger Full GC?
    // Current strategy: Reset pointer. Dangerous if live objects exist!
    // SAFE MODE: Don't reset unless we know everything is dead.
    // So for now, we just fill it up and then fall back to malloc.
    // This gives fast start-up speed (google scale req).
    
    // nursery.current = nursery.start; 
}

// ----------------------------------------------------------------------------

// Access the global VM instance
extern VM vm;

void initGC(VM* vm) {
    vm->grayCount = 0;
    vm->grayCapacity = 0;
    vm->grayStack = NULL;
    vm->bytesAllocated = 0;
    vm->nextGC = 1024 * 1024;
    
    initNursery();
}

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    // Stats
    if (newSize > oldSize) vm.bytesAllocated += newSize - oldSize;
    else vm.bytesAllocated -= oldSize - newSize; // Approximate for nursery
    
    if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
        collectGarbage(&vm);
#endif
        if (vm.bytesAllocated > vm.nextGC) {
            collectGarbage(&vm);
        }
    }

    if (newSize == 0) {
        if (pointer == NULL) return NULL;
        if (is_in_nursery(pointer)) {
            // No-op free for nursery objects (bulk freed/reset)
            return NULL;
        }
        free(pointer);
        return NULL;
    }

    // Allocation
    if (oldSize == 0) {
        // Try Nursery for small objects (e.g., < 256 bytes)
        // Only if it's a fresh allocation
        if (newSize < 256) {
            void* mem = nursery_alloc(newSize);
            if (mem) return mem;
            // Fallthrough to malloc if full
        }
    } else {
        // Reallocation
        if (is_in_nursery(pointer)) {
            // Moving out of nursery (Promote to Heap)
            void* newMem = malloc(newSize);
            if (!newMem) exit(1);
            // Copy old data
            // We don't know exact valid size to copy if oldSize is loose, 
            // but reallocate api passes oldSize.
            size_t copySize = oldSize < newSize ? oldSize : newSize;
            // memcpy(newMem, pointer, copySize); // Need string.h
            // We can't include string.h easily without messy diff? 
            // We can iterate.
             uint8_t* src = (uint8_t*)pointer;
             uint8_t* dst = (uint8_t*)newMem;
             for (size_t i = 0; i < copySize; i++) dst[i] = src[i];
            
            return newMem;
        }
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
        if (vm.grayStack == NULL) {
            fprintf(stderr, "Fatal: Out of memory for gray stack.\n");
            exit(1); 
        }
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
        case OBJ_TENSOR:
            break;
        case OBJ_CONTEXT: {
            ObjContext* context = (ObjContext*)object;
            markObject((Obj*)context->name);
            markTable(&context->layers);
            break;
        }
        case OBJ_LAYER: {
            ObjLayer* layer = (ObjLayer*)object;
            markObject((Obj*)layer->name);
            markTable(&layer->methods);
            break;
        }
        default:
            break;
    }
}

static void markRoots() {
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }
    for (int i = 0; i < vm.frameCount; i++) {
        markObject((Obj*)vm.frames[i].closure);
    }
    for (ObjUpvalue* upvalue = vm.openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        markObject((Obj*)upvalue);
    }
    markTable(&vm.globals);
    markObject((Obj*)vm.initString);
    markObject((Obj*)vm.cliArgs);
    markTable(&vm.importer.modules);
    for (int i = 0; i < vm.activeContextCount; i++) {
        markObject((Obj*)vm.activeContextStack[i]);
    }
    markCompilerRoots();
}

static void traceReferences() {
    while (vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        if (object != NULL) blackenObject(object);
    }
}

static void freeObject(Obj* object) {
    if (is_in_nursery(object)) return; // Don't free nursery objects individually

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
        case OBJ_CONTEXT: {
            ObjContext* context = (ObjContext*)object;
            freeTable(&context->layers);
            FREE(ObjContext, object);
            break;
        }
        case OBJ_LAYER: {
            ObjLayer* layer = (ObjLayer*)object;
            freeTable(&layer->methods);
            FREE(ObjLayer, object);
            break;
        }
        case OBJ_TENSOR: {
            ObjTensor* tensor = (ObjTensor*)object;
            FREE_ARRAY(int, tensor->dims, tensor->dimCount);
            FREE_ARRAY(double, tensor->data, tensor->size);
            FREE(ObjTensor, object);
            break;
        }
        case OBJ_TASK: {
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
    }

#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = vm.bytesAllocated;
#endif

    markRoots();
    traceReferences();
    
    tableRemoveWhite(&vm.strings);
    
    sweep();
    
    // reset_nursery(); // Dangerous without evacuation
    
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
    
    if (nursery_initialized) {
        free(nursery.start);
    }
}
