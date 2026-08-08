// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-25
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - JSON Module
 * Native C implementation of JSON parsing and stringification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"

extern VM vm;

// Helper to define native function in a module
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// Forward declarations
static Value parseJson(const char** cursor);

// Skip whitespace
static void skipWhitespace(const char** cursor) {
    while (isspace((unsigned char)**cursor)) (*cursor)++;
}

// Parse a JSON string (handling escapes)
static Value parseJsonString(const char** cursor) {
    (*cursor)++; // Skip opening quote
    const char* start = *cursor;
    size_t len = 0;
    
    while (**cursor != '\0' && **cursor != '"') {
        if (**cursor == '\\') {
            (*cursor)++;
            if (**cursor == '\0') break;
        }
        (*cursor)++;
    }
    
    len = (size_t)(*cursor - start);
    (*cursor)++; // Skip closing quote
    
    // Unescape the string
    char* result = (char*)malloc(len + 1);
    if (!result) return NIL_VAL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        char c = start[i];
        if (c == '\\' && i + 1 < len) {
            i++;
            switch (start[i]) {
                case '"': result[j++] = '"'; break;
                case '\\': result[j++] = '\\'; break;
                case '/': result[j++] = '/'; break;
                case 'b': result[j++] = '\b'; break;
                case 'f': result[j++] = '\f'; break;
                case 'n': result[j++] = '\n'; break;
                case 'r': result[j++] = '\r'; break;
                case 't': result[j++] = '\t'; break;
                case 'u':
                    if (i + 4 < len) {
                        char hex[5] = { start[i], start[i+1], start[i+2], start[i+3], '\0' };
                        result[j++] = (char)strtol(hex, NULL, 16);
                        i += 3;
                    }
                    break;
                default:
                    result[j++] = c;
                    break;
            }
        } else {
            result[j++] = c;
        }
    }
    result[j] = '\0';
    
    return OBJ_VAL(takeString(result, (int)j));
}

// Parse a JSON number
static Value parseJsonNumber(const char** cursor) {
    const char* start = *cursor;
    if (**cursor == '-') (*cursor)++;
    
    while (isdigit((unsigned char)**cursor)) (*cursor)++;
    
    if (**cursor == '.') {
        (*cursor)++;
        while (isdigit((unsigned char)**cursor)) (*cursor)++;
    }
    
    if (**cursor == 'e' || **cursor == 'E') {
        (*cursor)++;
        if (**cursor == '+' || **cursor == '-') (*cursor)++;
        while (isdigit((unsigned char)**cursor)) (*cursor)++;
    }
    
    double value = strtod(start, NULL);
    return NUMBER_VAL(value);
}

// Forward declaration for array/object parsing
static Value parseJsonValue(const char** cursor);

// Parse a JSON array
static Value parseJsonArray(const char** cursor) {
    (*cursor)++; // Skip '['
    skipWhitespace(cursor);
    
    ObjList* list = newList();
    push(&vm, OBJ_VAL(list));
    
    if (**cursor != ']') {
        while (1) {
            Value elem = parseJsonValue(cursor);
            if (IS_NIL(elem) && **cursor != ']' && **cursor != '\0') {
                // Parse error, but continue
            }
            list_append(list, elem);
            skipWhitespace(cursor);
            
            if (**cursor == ',') {
                (*cursor)++;
                skipWhitespace(cursor);
            } else {
                break;
            }
        }
    }
    
    if (**cursor == ']') (*cursor)++;
    
    return pop(&vm);
}

// Parse a JSON object
static Value parseJsonObject(const char** cursor) {
    (*cursor)++; // Skip '{'
    skipWhitespace(cursor);
    
    ObjDictionary* dict = newDictionary();
    push(&vm, OBJ_VAL(dict));
    
    if (**cursor != '}') {
        while (1) {
            skipWhitespace(cursor);
            if (**cursor != '"') break;
            
            Value keyVal = parseJsonString(cursor);
            if (!IS_STRING(keyVal)) {
                pop(&vm);
                return NIL_VAL;
            }
            
            skipWhitespace(cursor);
            if (**cursor != ':') {
                pop(&vm);
                return NIL_VAL;
            }
            (*cursor)++; // Skip ':'
            skipWhitespace(cursor);
            
            Value value = parseJsonValue(cursor);
            tableSet(&dict->items, AS_STRING(keyVal), value);
            
            skipWhitespace(cursor);
            if (**cursor == ',') {
                (*cursor)++;
                skipWhitespace(cursor);
            } else {
                break;
            }
        }
    }
    
    if (**cursor == '}') (*cursor)++;
    
    return pop(&vm);
}

// Parse any JSON value
static Value parseJsonValue(const char** cursor) {
    skipWhitespace(cursor);
    
    if (**cursor == '"') {
        return parseJsonString(cursor);
    } else if (**cursor == '{') {
        return parseJsonObject(cursor);
    } else if (**cursor == '[') {
        return parseJsonArray(cursor);
    } else if (strncmp(*cursor, "true", 4) == 0) {
        *cursor += 4;
        return BOOL_VAL(true);
    } else if (strncmp(*cursor, "false", 5) == 0) {
        *cursor += 5;
        return BOOL_VAL(false);
    } else if (strncmp(*cursor, "null", 4) == 0) {
        *cursor += 4;
        return NULL_VAL;
    } else if (**cursor == '-' || isdigit((unsigned char)**cursor)) {
        return parseJsonNumber(cursor);
    }
    
    return NIL_VAL;
}

// parse(str) -> Object
static Value native_json_parse(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char* cursor = AS_CSTRING(args[0]);
    Value result = parseJsonValue(&cursor);
    
    skipWhitespace(&cursor);
    if (*cursor != '\0') {
        // Partial parse - return what we got but it's technically malformed
        // For now, return the result anyway
    }
    
    return result;
}

// Helper to stringify a value to JSON using a simple dynamic buffer
typedef struct {
    char* data;
    size_t len;
    size_t cap;
} JsonBuf;

static void jsonBufInit(JsonBuf* buf) {
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

static void jsonBufFree(JsonBuf* buf) {
    if (buf->data) free(buf->data);
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

static void jsonBufAppend(JsonBuf* buf, const char* s, size_t n) {
    if (buf->len + n + 1 > buf->cap) {
        size_t newCap = (buf->len + n + 1) * 2;
        char* newData = (char*)realloc(buf->data, newCap);
        if (!newData) return;
        buf->data = newData;
        buf->cap = newCap;
    }
    memcpy(buf->data + buf->len, s, n);
    buf->len += n;
    buf->data[buf->len] = '\0';
}

static void jsonStringifyValue(Value val, JsonBuf* buf) {
    if (IS_NULL(val)) {
        jsonBufAppend(buf, "null", 4);
    } else if (IS_BOOL(val)) {
        if (AS_BOOL(val)) {
            jsonBufAppend(buf, "true", 4);
        } else {
            jsonBufAppend(buf, "false", 5);
        }
    } else if (IS_NUMBER(val)) {
        char num[64];
        snprintf(num, sizeof(num), "%.14g", AS_NUMBER(val));
        jsonBufAppend(buf, num, (int)strlen(num));
    } else if (IS_STRING(val)) {
        ObjString* str = AS_STRING(val);
        jsonBufAppend(buf, "\"", 1);
        for (int i = 0; i < str->length; i++) {
            char c = str->chars[i];
            switch (c) {
                case '"': jsonBufAppend(buf, "\\\"", 2); break;
                case '\\': jsonBufAppend(buf, "\\\\", 2); break;
                case '\b': jsonBufAppend(buf, "\\b", 2); break;
                case '\f': jsonBufAppend(buf, "\\f", 2); break;
                case '\n': jsonBufAppend(buf, "\\n", 2); break;
                case '\r': jsonBufAppend(buf, "\\r", 2); break;
                case '\t': jsonBufAppend(buf, "\\t", 2); break;
                default:
                    if ((unsigned char)c < 0x20) {
                        char esc[7];
                        snprintf(esc, sizeof(esc), "\\u%04x", (unsigned char)c);
                        jsonBufAppend(buf, esc, 6);
                    } else {
                        jsonBufAppend(buf, &c, 1);
                    }
                    break;
            }
        }
        jsonBufAppend(buf, "\"", 1);
    } else if (IS_LIST(val)) {
        ObjList* list = AS_LIST(val);
        jsonBufAppend(buf, "[", 1);
        for (int i = 0; i < list->count; i++) {
            if (i > 0) jsonBufAppend(buf, ",", 1);
            jsonStringifyValue(list->items[i], buf);
        }
        jsonBufAppend(buf, "]", 1);
    } else if (IS_DICTIONARY(val)) {
        ObjDictionary* dict = AS_DICTIONARY(val);
        jsonBufAppend(buf, "{", 1);
        bool first = true;
        for (int i = 0; i < dict->items.capacity; i++) {
            Entry* entry = &dict->items.entries[i];
            if (entry->key != NULL) {
                if (!first) jsonBufAppend(buf, ",", 1);
                first = false;
                jsonStringifyValue(OBJ_VAL(entry->key), buf);
                jsonBufAppend(buf, ":", 1);
                jsonStringifyValue(entry->value, buf);
            }
        }
        jsonBufAppend(buf, "}", 1);
    } else {
        jsonBufAppend(buf, "null", 4);
    }
}

// stringify(val) -> String
static Value native_json_stringify(int argCount, Value* args) {
    if (argCount < 1) return OBJ_VAL(copyString("", 0));
    
    JsonBuf buf;
    jsonBufInit(&buf);
    jsonStringifyValue(args[0], &buf);
    
    Value result = OBJ_VAL(takeString(buf.data, (int)buf.len));
    jsonBufFree(&buf);
    return result;
}

ObjModule* create_std_json_module() {
    ObjString* name = copyString("std.native.json", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "parse", native_json_parse);
    defineModuleFn(module, "stringify", native_json_stringify);

    pop(&vm);
    pop(&vm);
    return module;
}
