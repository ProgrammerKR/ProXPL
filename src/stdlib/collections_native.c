// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-25  |  Upgraded: 2026-04-27
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include "../../include/memory.h"

extern VM vm;

static void defineModuleFn(ObjModule* module, const char* name, NativeFn fn) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(fn)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// Helper: push list item (avoids GC race)
static void list_append(ObjList* list, Value val) {
    if (list->capacity < list->count + 1) {
        int old = list->capacity;
        list->capacity = GROW_CAPACITY(old);
        list->items = GROW_ARRAY(Value, list->items, old, list->capacity);
    }
    list->items[list->count++] = val;
}

// ---------- map(list, fn) ----------
static Value native_col_map(int argCount, Value* args) {
    if (argCount < 2 || !IS_LIST(args[0])) return NIL_VAL;
    ObjList* src = AS_LIST(args[0]);
    Value fn = args[1];

    ObjList* result = newList();
    push(&vm, OBJ_VAL(result));

    for (int i = 0; i < src->count; i++) {
        Value item = src->items[i];
        Value mapped;

        if (IS_NATIVE(fn)) {
            mapped = AS_NATIVE(fn)(1, &item);
        } else {
            // ProXPL closure – call via stack (simplified)
            // Push fn, then arg, then call
            push(&vm, fn);
            push(&vm, item);
            // We can't invoke ProXPL closures easily from native without
            // the interpreter loop, so we store the raw value for now.
            // A full callValue() integration would require vm loop re-entry.
            pop(&vm); // item
            pop(&vm); // fn
            mapped = item;
        }

        list_append(result, mapped);
    }

    return pop(&vm);
}

// ---------- filter(list, fn) ----------
static Value native_col_filter(int argCount, Value* args) {
    if (argCount < 2 || !IS_LIST(args[0])) return NIL_VAL;
    ObjList* src = AS_LIST(args[0]);
    Value fn = args[1];

    ObjList* result = newList();
    push(&vm, OBJ_VAL(result));

    for (int i = 0; i < src->count; i++) {
        Value item = src->items[i];
        bool keep = false;

        if (IS_NATIVE(fn)) {
            Value r = AS_NATIVE(fn)(1, &item);
            keep = IS_BOOL(r) ? AS_BOOL(r) : !IS_NIL(r);
        } else {
            keep = true; // passthrough (ProXPL closure called at higher level)
        }

        if (keep) list_append(result, item);
    }

    return pop(&vm);
}

// ---------- reduce(list, fn, initial) ----------
static Value native_col_reduce(int argCount, Value* args) {
    if (argCount < 3 || !IS_LIST(args[0])) return NIL_VAL;
    ObjList* src = AS_LIST(args[0]);
    Value fn  = args[1];
    Value acc = args[2];

    for (int i = 0; i < src->count; i++) {
        if (IS_NATIVE(fn)) {
            Value pair[2] = { acc, src->items[i] };
            acc = AS_NATIVE(fn)(2, pair);
        }
        // ProXPL closures: high-level wrapper in .prox handles it
    }

    return acc;
}

// ---------- flatten(list) ----------
static Value native_col_flatten(int argCount, Value* args) {
    if (argCount < 1 || !IS_LIST(args[0])) return NIL_VAL;
    ObjList* result = newList();
    push(&vm, OBJ_VAL(result));

    ObjList* src = AS_LIST(args[0]);
    for (int i = 0; i < src->count; i++) {
        Value item = src->items[i];
        if (IS_LIST(item)) {
            ObjList* inner = AS_LIST(item);
            for (int j = 0; j < inner->count; j++) {
                list_append(result, inner->items[j]);
            }
        } else {
            list_append(result, item);
        }
    }

    return pop(&vm);
}

// ---------- zip(a, b) -> list of [a[i], b[i]] ----------
static Value native_col_zip(int argCount, Value* args) {
    if (argCount < 2 || !IS_LIST(args[0]) || !IS_LIST(args[1])) return NIL_VAL;
    ObjList* a = AS_LIST(args[0]);
    ObjList* b = AS_LIST(args[1]);
    int n = a->count < b->count ? a->count : b->count;

    ObjList* result = newList();
    push(&vm, OBJ_VAL(result));

    for (int i = 0; i < n; i++) {
        ObjList* pair = newList();
        push(&vm, OBJ_VAL(pair));
        list_append(pair, a->items[i]);
        list_append(pair, b->items[i]);
        list_append(result, OBJ_VAL(pair));
        pop(&vm); // pair
    }

    return pop(&vm);
}

// ---------- range(start, stop, step) ----------
static Value native_col_range(int argCount, Value* args) {
    double start = 0, stop = 0, step = 1;
    if (argCount == 1 && IS_NUMBER(args[0])) {
        stop = AS_NUMBER(args[0]);
    } else if (argCount >= 2 && IS_NUMBER(args[0]) && IS_NUMBER(args[1])) {
        start = AS_NUMBER(args[0]);
        stop  = AS_NUMBER(args[1]);
        if (argCount >= 3 && IS_NUMBER(args[2])) step = AS_NUMBER(args[2]);
    } else {
        return NIL_VAL;
    }
    if (step == 0) step = 1;

    ObjList* result = newList();
    push(&vm, OBJ_VAL(result));

    if (step > 0) {
        for (double v = start; v < stop; v += step) list_append(result, NUMBER_VAL(v));
    } else {
        for (double v = start; v > stop; v += step) list_append(result, NUMBER_VAL(v));
    }

    return pop(&vm);
}

// ---------- slice(list, start, end) ----------
static Value native_col_slice(int argCount, Value* args) {
    if (argCount < 3 || !IS_LIST(args[0]) || !IS_NUMBER(args[1]) || !IS_NUMBER(args[2])) return NIL_VAL;
    ObjList* src = AS_LIST(args[0]);
    int start = (int)AS_NUMBER(args[1]);
    int end   = (int)AS_NUMBER(args[2]);
    if (start < 0) start = 0;
    if (end > src->count) end = src->count;

    ObjList* result = newList();
    push(&vm, OBJ_VAL(result));
    for (int i = start; i < end; i++) list_append(result, src->items[i]);
    return pop(&vm);
}

// ---------- concat(a, b) ----------
static Value native_col_concat(int argCount, Value* args) {
    if (argCount < 2 || !IS_LIST(args[0]) || !IS_LIST(args[1])) return NIL_VAL;
    ObjList* a = AS_LIST(args[0]);
    ObjList* b = AS_LIST(args[1]);

    ObjList* result = newList();
    push(&vm, OBJ_VAL(result));
    for (int i = 0; i < a->count; i++) list_append(result, a->items[i]);
    for (int i = 0; i < b->count; i++) list_append(result, b->items[i]);
    return pop(&vm);
}

// ---------- sum(list) ----------
static Value native_col_sum(int argCount, Value* args) {
    if (argCount < 1 || !IS_LIST(args[0])) return NUMBER_VAL(0);
    ObjList* list = AS_LIST(args[0]);
    double total = 0;
    for (int i = 0; i < list->count; i++) {
        if (IS_NUMBER(list->items[i])) total += AS_NUMBER(list->items[i]);
    }
    return NUMBER_VAL(total);
}

// ---------- any(list, fn) ----------
static Value native_col_any(int argCount, Value* args) {
    if (argCount < 2 || !IS_LIST(args[0])) return BOOL_VAL(false);
    ObjList* list = AS_LIST(args[0]);
    Value fn = args[1];
    for (int i = 0; i < list->count; i++) {
        if (IS_NATIVE(fn)) {
            Value r = AS_NATIVE(fn)(1, &list->items[i]);
            if (IS_BOOL(r) && AS_BOOL(r)) return BOOL_VAL(true);
        }
    }
    return BOOL_VAL(false);
}

// ---------- all(list, fn) ----------
static Value native_col_all(int argCount, Value* args) {
    if (argCount < 2 || !IS_LIST(args[0])) return BOOL_VAL(false);
    ObjList* list = AS_LIST(args[0]);
    Value fn = args[1];
    for (int i = 0; i < list->count; i++) {
        if (IS_NATIVE(fn)) {
            Value r = AS_NATIVE(fn)(1, &list->items[i]);
            if (!IS_BOOL(r) || !AS_BOOL(r)) return BOOL_VAL(false);
        }
    }
    return BOOL_VAL(true);
}

// ---------- first(list) / last(list) ----------
static Value native_col_first(int argCount, Value* args) {
    if (argCount < 1 || !IS_LIST(args[0])) return NIL_VAL;
    ObjList* l = AS_LIST(args[0]);
    return l->count > 0 ? l->items[0] : NIL_VAL;
}

static Value native_col_last(int argCount, Value* args) {
    if (argCount < 1 || !IS_LIST(args[0])) return NIL_VAL;
    ObjList* l = AS_LIST(args[0]);
    return l->count > 0 ? l->items[l->count - 1] : NIL_VAL;
}

// ---------- count(list, val) ----------
static Value native_col_count(int argCount, Value* args) {
    if (argCount < 2 || !IS_LIST(args[0])) return NUMBER_VAL(0);
    ObjList* list = AS_LIST(args[0]);
    Value target = args[1];
    int count = 0;
    for (int i = 0; i < list->count; i++) {
        Value item = list->items[i];
        bool eq = false;
        if (IS_NUMBER(item) && IS_NUMBER(target)) eq = AS_NUMBER(item) == AS_NUMBER(target);
        else if (IS_BOOL(item) && IS_BOOL(target)) eq = AS_BOOL(item) == AS_BOOL(target);
        else if (IS_STRING(item) && IS_STRING(target)) {
            eq = strcmp(AS_CSTRING(item), AS_CSTRING(target)) == 0;
        }
        if (eq) count++;
    }
    return NUMBER_VAL((double)count);
}

ObjModule* create_std_collections_module() {
    ObjString* name = copyString("std.native.collections", 22);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));

    defineModuleFn(module, "map",     native_col_map);
    defineModuleFn(module, "filter",  native_col_filter);
    defineModuleFn(module, "reduce",  native_col_reduce);
    defineModuleFn(module, "flatten", native_col_flatten);
    defineModuleFn(module, "zip",     native_col_zip);
    defineModuleFn(module, "range",   native_col_range);
    defineModuleFn(module, "slice",   native_col_slice);
    defineModuleFn(module, "concat",  native_col_concat);
    defineModuleFn(module, "sum",     native_col_sum);
    defineModuleFn(module, "any",     native_col_any);
    defineModuleFn(module, "all",     native_col_all);
    defineModuleFn(module, "first",   native_col_first);
    defineModuleFn(module, "last",    native_col_last);
    defineModuleFn(module, "count",   native_col_count);

    pop(&vm); // module
    pop(&vm); // name
    return module;
}
