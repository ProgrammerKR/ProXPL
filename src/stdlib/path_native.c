// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2026-04-27
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

/*
 * ProXPL Standard Library - Path Module
 * Cross-platform path manipulation utilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #define PATH_SEP '\\'
  #define PATH_SEP_STR "\\"
#else
  #define PATH_SEP '/'
  #define PATH_SEP_STR "/"
#endif

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"

extern VM vm;

static void defineModuleFn(ObjModule* module, const char* name, NativeFn fn) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(fn)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// Determine if char is a path separator (handle both / and \ on Windows)
static bool is_sep(char c) {
#ifdef _WIN32
    return c == '/' || c == '\\';
#else
    return c == '/';
#endif
}

// path.join(a, b, ...) -> string
static Value native_path_join(int argCount, Value* args) {
    if (argCount == 0) return OBJ_VAL(copyString("", 0));

    char buf[4096];
    buf[0] = '\0';
    int pos = 0;

    for (int i = 0; i < argCount; i++) {
        if (!IS_STRING(args[i])) continue;
        const char* part = AS_CSTRING(args[i]);
        int partLen = (int)strlen(part);
        if (partLen == 0) continue;

        // If segment is absolute and not the first, reset
        bool isAbsolute = is_sep(part[0]);
#ifdef _WIN32
        // Also absolute if like "C:\"
        if (partLen >= 2 && part[1] == ':') isAbsolute = true;
#endif
        if (isAbsolute && i != 0) {
            pos = 0;
            buf[0] = '\0';
        }

        // Add separator if needed
        if (pos > 0 && !is_sep(buf[pos-1]) && !is_sep(part[0])) {
            if (pos < (int)sizeof(buf) - 2) buf[pos++] = PATH_SEP;
        }

        int copyLen = partLen;
        if (pos + copyLen >= (int)sizeof(buf) - 1) copyLen = (int)sizeof(buf) - 1 - pos;
        memcpy(buf + pos, part, copyLen);
        pos += copyLen;
        buf[pos] = '\0';
    }

    return OBJ_VAL(copyString(buf, pos));
}

// path.dirname(p) -> string
static Value native_path_dirname(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return OBJ_VAL(copyString(".", 1));
    const char* p = AS_CSTRING(args[0]);
    int len = (int)strlen(p);

    // Strip trailing separators
    int end = len - 1;
    while (end > 0 && is_sep(p[end])) end--;

    // Find last separator
    int last_sep = -1;
    for (int i = end; i >= 0; i--) {
        if (is_sep(p[i])) { last_sep = i; break; }
    }

    if (last_sep < 0) return OBJ_VAL(copyString(".", 1));
    if (last_sep == 0) return OBJ_VAL(copyString(PATH_SEP_STR, 1));
    return OBJ_VAL(copyString(p, last_sep));
}

// path.basename(p) -> string  (optionally strip extension with second arg)
static Value native_path_basename(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return OBJ_VAL(copyString("", 0));
    const char* p   = AS_CSTRING(args[0]);
    int len = (int)strlen(p);

    // Strip trailing separators
    int end = len - 1;
    while (end > 0 && is_sep(p[end])) end--;

    // Find last separator
    int start = 0;
    for (int i = end; i >= 0; i--) {
        if (is_sep(p[i])) { start = i + 1; break; }
    }

    int nameLen = end - start + 1;
    if (nameLen <= 0) return OBJ_VAL(copyString("", 0));

    // Optional: strip extension
    if (argCount >= 2 && IS_STRING(args[1])) {
        const char* ext = AS_CSTRING(args[1]);
        int extLen = (int)strlen(ext);
        if (extLen > 0 && nameLen > extLen) {
            if (strncmp(p + start + nameLen - extLen, ext, extLen) == 0) {
                nameLen -= extLen;
            }
        }
    }

    return OBJ_VAL(copyString(p + start, nameLen));
}

// path.extname(p) -> string  (e.g. ".prox")
static Value native_path_extname(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return OBJ_VAL(copyString("", 0));
    const char* p   = AS_CSTRING(args[0]);
    int len = (int)strlen(p);

    // Find last dot after last separator
    int last_sep = -1;
    for (int i = len - 1; i >= 0; i--) {
        if (is_sep(p[i])) { last_sep = i; break; }
    }

    for (int i = len - 1; i > last_sep + 1; i--) {
        if (p[i] == '.') return OBJ_VAL(copyString(p + i, len - i));
    }
    return OBJ_VAL(copyString("", 0));
}

// path.is_absolute(p) -> bool
static Value native_path_is_absolute(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    const char* p = AS_CSTRING(args[0]);
#ifdef _WIN32
    int len = (int)strlen(p);
    if (is_sep(p[0])) return BOOL_VAL(true);
    if (len >= 2 && p[1] == ':') return BOOL_VAL(true);
    return BOOL_VAL(false);
#else
    return BOOL_VAL(p[0] == '/');
#endif
}

// path.exists(p) -> bool
static Value native_path_exists(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    struct stat st;
    return BOOL_VAL(stat(AS_CSTRING(args[0]), &st) == 0);
}

// path.is_file(p) -> bool
static Value native_path_is_file(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    struct stat st;
    if (stat(AS_CSTRING(args[0]), &st) != 0) return BOOL_VAL(false);
    return BOOL_VAL((st.st_mode & S_IFMT) == S_IFREG);
}

// path.is_dir(p) -> bool
static Value native_path_is_dir(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    struct stat st;
    if (stat(AS_CSTRING(args[0]), &st) != 0) return BOOL_VAL(false);
    return BOOL_VAL((st.st_mode & S_IFMT) == S_IFDIR);
}

// path.normalize(p) -> string  (collapse . and ..)
static Value native_path_normalize(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return OBJ_VAL(copyString(".", 1));
    const char* in = AS_CSTRING(args[0]);
    int inLen = (int)strlen(in);

    // Tokenise by separator, build component list, handle . and ..
    char** parts = (char**)malloc(sizeof(char*) * (inLen + 1));
    int count = 0;

    // Copy to mutable buffer
    char* tmp = (char*)malloc(inLen + 1);
    memcpy(tmp, in, inLen + 1);

    bool absolute = is_sep(tmp[0]);
#ifdef _WIN32
    if (inLen >= 2 && tmp[1] == ':') absolute = true;
#endif

    char* tok = strtok(tmp, "/\\");
    while (tok) {
        if (strcmp(tok, ".") == 0) {
            // skip
        } else if (strcmp(tok, "..") == 0) {
            if (count > 0) count--;
        } else {
            parts[count++] = tok;
        }
        tok = strtok(NULL, "/\\");
    }

    // Re-assemble
    char out[4096];
    int pos = 0;
    if (absolute) out[pos++] = PATH_SEP;
    for (int i = 0; i < count; i++) {
        int pLen = (int)strlen(parts[i]);
        if (i > 0 || absolute) { /* skip leading sep already added for absolute */ }
        if (i > 0) out[pos++] = PATH_SEP;
        memcpy(out + pos, parts[i], pLen);
        pos += pLen;
    }
    if (pos == 0) { out[pos++] = '.'; }
    out[pos] = '\0';

    free(tmp);
    free(parts);
    return OBJ_VAL(copyString(out, pos));
}

ObjModule* create_std_path_module() {
    ObjString* name = copyString("std.native.path", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));

    defineModuleFn(module, "join",        native_path_join);
    defineModuleFn(module, "dirname",     native_path_dirname);
    defineModuleFn(module, "basename",    native_path_basename);
    defineModuleFn(module, "extname",     native_path_extname);
    defineModuleFn(module, "is_absolute", native_path_is_absolute);
    defineModuleFn(module, "exists",      native_path_exists);
    defineModuleFn(module, "is_file",     native_path_is_file);
    defineModuleFn(module, "is_dir",      native_path_is_dir);
    defineModuleFn(module, "normalize",   native_path_normalize);

    pop(&vm); // module
    pop(&vm); // name
    return module;
}
