// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2026-04-27
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - Buffer Module
 * Binary buffer: alloc, read/write bytes, hex dump.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"

extern VM vm;

// Internal buffer structure stored inside ObjForeign->library pointer
typedef struct {
    uint8_t* data;
    int      size;
    int      capacity;
} ProxBuffer;

// Helper: create / resize
static ProxBuffer* buf_new(int capacity) {
    ProxBuffer* b = (ProxBuffer*)malloc(sizeof(ProxBuffer));
    if (!b) return NULL;
    b->data = (uint8_t*)calloc(capacity, 1);
    b->size = 0;
    b->capacity = capacity;
    return b;
}

static void buf_free_cb(void* ptr) {
    if (!ptr) return;
    ProxBuffer* b = (ProxBuffer*)ptr;
    free(b->data);
    free(b);
}

static void defineModuleFn(ObjModule* module, const char* name, NativeFn fn) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(fn)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// buffer.alloc(size) -> Buffer
static Value native_buf_alloc(int argCount, Value* args) {
    int sz = (argCount >= 1 && IS_NUMBER(args[0])) ? (int)AS_NUMBER(args[0]) : 64;
    if (sz <= 0) sz = 64;
    ProxBuffer* b = buf_new(sz);
    if (!b) return NIL_VAL;
    ObjString* tag = copyString("Buffer", 6);
    push(&vm, OBJ_VAL(tag));
    ObjForeign* f = newForeign(tag, (void*)b, NULL);
    pop(&vm);
    return OBJ_VAL(f);
}

// buffer.write_byte(buf, byte) -> nil
static Value native_buf_write_byte(int argCount, Value* args) {
    if (argCount < 2 || !IS_FOREIGN(args[0]) || !IS_NUMBER(args[1])) return NIL_VAL;
    ProxBuffer* b = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    uint8_t byte = (uint8_t)((int)AS_NUMBER(args[1]) & 0xFF);
    if (b->size >= b->capacity) {
        b->capacity *= 2;
        b->data = (uint8_t*)realloc(b->data, b->capacity);
    }
    b->data[b->size++] = byte;
    return NIL_VAL;
}

// buffer.read_byte(buf, index) -> number
static Value native_buf_read_byte(int argCount, Value* args) {
    if (argCount < 2 || !IS_FOREIGN(args[0]) || !IS_NUMBER(args[1])) return NUMBER_VAL(-1);
    ProxBuffer* b = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    int idx = (int)AS_NUMBER(args[1]);
    if (idx < 0 || idx >= b->size) return NUMBER_VAL(-1);
    return NUMBER_VAL((double)b->data[idx]);
}

// buffer.size(buf) -> number
static Value native_buf_size(int argCount, Value* args) {
    if (argCount < 1 || !IS_FOREIGN(args[0])) return NUMBER_VAL(0);
    ProxBuffer* b = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    return NUMBER_VAL((double)b->size);
}

// buffer.write_string(buf, str) -> nil
static Value native_buf_write_str(int argCount, Value* args) {
    if (argCount < 2 || !IS_FOREIGN(args[0]) || !IS_STRING(args[1])) return NIL_VAL;
    ProxBuffer* b  = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    ObjString*  s  = AS_STRING(args[1]);
    int needed = b->size + s->length;
    while (b->capacity < needed) {
        b->capacity *= 2;
        b->data = (uint8_t*)realloc(b->data, b->capacity);
    }
    memcpy(b->data + b->size, s->chars, s->length);
    b->size += s->length;
    return NIL_VAL;
}

// buffer.to_string(buf) -> string
static Value native_buf_to_string(int argCount, Value* args) {
    if (argCount < 1 || !IS_FOREIGN(args[0])) return OBJ_VAL(copyString("", 0));
    ProxBuffer* b = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    return OBJ_VAL(copyString((const char*)b->data, b->size));
}

// buffer.hex_dump(buf) -> string  (like "48 65 6c 6c 6f")
static Value native_buf_hex_dump(int argCount, Value* args) {
    if (argCount < 1 || !IS_FOREIGN(args[0])) return OBJ_VAL(copyString("", 0));
    ProxBuffer* b = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    if (b->size == 0) return OBJ_VAL(copyString("", 0));

    // Each byte: "XX " = 3 chars, last has no space
    int outLen = b->size * 3;
    char* out = (char*)malloc(outLen + 1);
    for (int i = 0; i < b->size; i++) {
        sprintf(out + i * 3, "%02x", b->data[i]);
        if (i < b->size - 1) out[i * 3 + 2] = ' ';
    }
    out[outLen] = '\0';
    Value result = OBJ_VAL(copyString(out, outLen));
    free(out);
    return result;
}

// buffer.clear(buf) -> nil
static Value native_buf_clear(int argCount, Value* args) {
    if (argCount < 1 || !IS_FOREIGN(args[0])) return NIL_VAL;
    ProxBuffer* b = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    memset(b->data, 0, b->capacity);
    b->size = 0;
    return NIL_VAL;
}

// buffer.slice(buf, start, length) -> string
static Value native_buf_slice(int argCount, Value* args) {
    if (argCount < 3 || !IS_FOREIGN(args[0]) || !IS_NUMBER(args[1]) || !IS_NUMBER(args[2])) {
        return OBJ_VAL(copyString("", 0));
    }
    ProxBuffer* b   = (ProxBuffer*)AS_FOREIGN(args[0])->library;
    int start  = (int)AS_NUMBER(args[1]);
    int length = (int)AS_NUMBER(args[2]);
    if (start < 0) start = 0;
    if (start >= b->size) return OBJ_VAL(copyString("", 0));
    if (start + length > b->size) length = b->size - start;
    if (length <= 0) return OBJ_VAL(copyString("", 0));
    return OBJ_VAL(copyString((const char*)(b->data + start), length));
}

ObjModule* create_std_buffer_module() {
    ObjString* name = copyString("std.native.buffer", 17);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));

    defineModuleFn(module, "alloc",        native_buf_alloc);
    defineModuleFn(module, "write_byte",   native_buf_write_byte);
    defineModuleFn(module, "read_byte",    native_buf_read_byte);
    defineModuleFn(module, "size",         native_buf_size);
    defineModuleFn(module, "write_string", native_buf_write_str);
    defineModuleFn(module, "to_string",    native_buf_to_string);
    defineModuleFn(module, "hex_dump",     native_buf_hex_dump);
    defineModuleFn(module, "clear",        native_buf_clear);
    defineModuleFn(module, "slice",        native_buf_slice);

    pop(&vm); // module
    pop(&vm); // name
    return module;
}
