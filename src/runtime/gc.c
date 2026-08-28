// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
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

void initGC(VM* pvm) {
    pvm->grayCount = 0;
    pvm->grayCapacity = 0;
    pvm->grayStack = NULL;
    pvm->bytesAllocated = 0;
    pvm->nextGC = 1024 * 1024;
}

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize > oldSize) {
        vm.bytesAllocated += (newSize - oldSize);
    } else {
        size_t diff = oldSize - newSize;
        if (vm.bytesAllocated >= diff) {
            vm.bytesAllocated -= diff;
        } else {
            vm.bytesAllocated = 0;
        }
    }
    
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
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    if (result == NULL) {
        fprintf(stderr, "Fatal: Out of memory.\n");
        exit(1);
    }
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
        case OBJ_INTENT: {
            ObjIntent* intent = (ObjIntent*)object;
            markObject((Obj*)intent->name);
            for (int i = 0; i < intent->resolverCount; i++) {
                markObject((Obj*)intent->resolvers[i]);
            }
            break;
        }
        case OBJ_RESOLVER: {
            ObjResolver* resolver = (ObjResolver*)object;
            markObject((Obj*)resolver->name);
            markObject((Obj*)resolver->handler);
            break;
        }
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
        case OBJ_DICTIONARY: {
            struct ObjDictionary* dict = (struct ObjDictionary*)object;
            markTable(&dict->items);
            break;
        }
        case OBJ_TENSOR:
            break;
        case OBJ_ACTOR: {
            ObjActor* actor = (ObjActor*)object;
            markObject((Obj*)actor->name);
            markTable(&actor->fields);
            ObjMessage *msg = actor->mailboxHead;
            while(msg != NULL) {
                markValue(msg->payload);
                markValue(msg->sender);
                msg = msg->next;
            }
            break;
        }
        case OBJ_CHANNEL: {
            ObjChannel* channel = (ObjChannel*)object;
            if (channel->buffer) {
                for (int i=0; i<channel->capacity; i++) {
                    markValue(channel->buffer[i]);
                }
            }
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

#ifdef DEBUG_LOG_GC
    printf("%p free ", (void*)object);
    printObject(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            reallocate(object, sizeof(ObjString) + string->length + 1, 0);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            if (function->cache != NULL) {
                reallocate(function->cache, sizeof(GICEntry) * function->chunk.count, 0);
            }
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
            if (closure->upvalues != NULL && closure->upvalueCount > 0) {
                FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
            }
            FREE(ObjClosure, object);
            break;
        }
        case OBJ_UPVALUE:
            FREE(ObjUpvalue, object);
            break;
        case OBJ_CLASS: {
            struct ObjClass* klass = (struct ObjClass*)object;
            freeTable(&klass->methods);
            if (klass->interfaces != NULL) {
                FREE_ARRAY(Value, klass->interfaces, klass->interfaceCount);
            }
            FREE(struct ObjClass, object);
            break;
        }
        case OBJ_INTERFACE: {
            ObjInterface* interface = (ObjInterface*)object;
            freeTable(&interface->methods);
            FREE(ObjInterface, object);
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
        case OBJ_INTENT: {
            ObjIntent* intent = (ObjIntent*)object;
            if (intent->resolvers) {
                FREE_ARRAY(ObjClosure*, intent->resolvers, intent->resolverCapacity);
            }
            FREE(ObjIntent, object);
            break;
        }
        case OBJ_RESOLVER: {
            FREE(ObjResolver, object);
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
        case OBJ_ACTOR: {
            ObjActor* actor = (ObjActor*)object;
            freeTable(&actor->fields);
            ObjMessage *msg = actor->mailboxHead;
            while(msg != NULL) {
                ObjMessage *next = msg->next;
                FREE(ObjMessage, msg);
                msg = next;
            }
            FREE(ObjActor, object);
            break;
        }
        case OBJ_CHANNEL: {
            ObjChannel* channel = (ObjChannel*)object;
            if (channel->buffer) {
                FREE_ARRAY(Value, channel->buffer, channel->capacity);
            }
            FREE(ObjChannel, object);
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
    
    if (vm.metrics.profileMode) {
        vm.metrics.gcCycles++;
    }

#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
#endif
    size_t before = vm.bytesAllocated;

#ifdef _WIN32
    // Basic cross-platform timing could be added here; using a simple clock() for now
#endif
    clock_t start_time = clock();

    markRoots();
    traceReferences();
    
    tableRemoveWhite(&vm.strings);
    
    sweep();
    
    vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

    if (vm.metrics.profileMode) {
        clock_t end_time = clock();
        vm.metrics.totalGCPause += (double)(end_time - start_time) / CLOCKS_PER_SEC;
    }

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
