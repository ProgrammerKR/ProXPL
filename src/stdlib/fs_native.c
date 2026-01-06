// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-22
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
<<<<<<< HEAD
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define mkdir(p, m) _mkdir(p)
#define rmdir _rmdir
#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & _S_IFMT) == _S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFMT) == _S_IFDIR)
#endif
#else
#include <unistd.h>
#include <dirent.h>
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#endif
=======
>>>>>>> fix-ci-build

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include "../../include/memory.h"

// Access VM
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

// --------------------------------------------------
<<<<<<< HEAD
// std.native.fs Implementation
// --------------------------------------------------

// read_file(path) -> String or Null
static Value fs_read_file(int argCount, Value* args) {
=======
// std.fs Implementation
// --------------------------------------------------

// readFile(path) -> String or Null
static Value fs_readFile(int argCount, Value* args) {
>>>>>>> fix-ci-build
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "rb");
    if (!file) return NIL_VAL;
    
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    
    char* buffer = (char*)malloc(fileSize + 1);
    if (!buffer) {
        fclose(file);
        return NIL_VAL;
    }
    
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    
<<<<<<< HEAD
    return OBJ_VAL(takeString(buffer, (int)bytesRead));
}

// write_file(path, content) -> Bool
static Value fs_write_file(int argCount, Value* args) {
=======
    Value result = OBJ_VAL(takeString(buffer, (int)bytesRead));
    return result;
}

// writeFile(path, content) -> Bool
static Value fs_writeFile(int argCount, Value* args) {
>>>>>>> fix-ci-build
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return BOOL_VAL(false);
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "w");
    if (!file) return BOOL_VAL(false);
    
    ObjString* content = AS_STRING(args[1]);
    fwrite(content->chars, 1, content->length, file);
    fclose(file);
    return BOOL_VAL(true);
}

<<<<<<< HEAD
// append_file(path, content) -> Bool
static Value fs_append_file(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return BOOL_VAL(false);
    
    const char* path = AS_CSTRING(args[0]);
    FILE* file = fopen(path, "a");
    if (!file) return BOOL_VAL(false);
    
    ObjString* content = AS_STRING(args[1]);
    fwrite(content->chars, 1, content->length, file);
    fclose(file);
    return BOOL_VAL(true);
}

=======
>>>>>>> fix-ci-build
// exists(path) -> Bool
static Value fs_exists(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    
<<<<<<< HEAD
    struct stat buffer;   
    return BOOL_VAL(stat(AS_CSTRING(args[0]), &buffer) == 0);
}

// remove(path) -> Bool
static Value fs_remove(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    return BOOL_VAL(remove(AS_CSTRING(args[0])) == 0);
}

// metadata(path) -> Map { "size": int, "exists": bool }
// Note: Returning a map implies we can create one. If VM doesn't expose map creation easily here,
// we might iterate. For now, let's just return size as integer or -1 if not found.
// Or if the VM has native map support exposed. 
// Assuming `newMap` or similar is not easily available without including more headers, 
// I'll stick to returning file size for now or NIL.
// Wait, libraries usually return objects. I'll return the size (number).
static Value fs_metadata(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    struct stat st;
    if (stat(AS_CSTRING(args[0]), &st) == 0) {
        return NUMBER_VAL((double)st.st_size);
    }
    return NIL_VAL;
}

// mkdir(path) -> Bool
static Value fs_mkdir(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    return BOOL_VAL(mkdir(AS_CSTRING(args[0]), 0777) == 0);
}

// rmdir(path) -> Bool
static Value fs_rmdir(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    return BOOL_VAL(rmdir(AS_CSTRING(args[0])) == 0);
}

// is_file(path) -> Bool
static Value fs_is_file(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    struct stat st;
    if (stat(AS_CSTRING(args[0]), &st) == 0) {
        return BOOL_VAL(S_ISREG(st.st_mode));
=======
    FILE* file = fopen(AS_CSTRING(args[0]), "r");
    if (file) {
        fclose(file);
        return BOOL_VAL(true);
>>>>>>> fix-ci-build
    }
    return BOOL_VAL(false);
}

<<<<<<< HEAD
// is_dir(path) -> Bool
static Value fs_is_dir(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return BOOL_VAL(false);
    struct stat st;
    if (stat(AS_CSTRING(args[0]), &st) == 0) {
        return BOOL_VAL(S_ISDIR(st.st_mode));
    }
    return BOOL_VAL(false);
}

// copy(src, dest) -> Bool
static Value fs_copy(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return BOOL_VAL(false);
    
    FILE* src = fopen(AS_CSTRING(args[0]), "rb");
    if (!src) return BOOL_VAL(false);
    
    FILE* dest = fopen(AS_CSTRING(args[1]), "wb");
    if (!dest) {
        fclose(src);
        return BOOL_VAL(false);
    }
    
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
        if (fwrite(buf, 1, n, dest) != n) {
            fclose(src);
            fclose(dest);
            return BOOL_VAL(false);
        }
    }
    
    fclose(src);
    fclose(dest);
    return BOOL_VAL(true);
}

// listdir(path) -> List<String>
static Value fs_listdir(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    const char* path = AS_CSTRING(args[0]);
    
    ObjList* list = newList();
    push(&vm, OBJ_VAL(list)); // GC protect

#ifdef _WIN32
    char searchPath[1024];
    snprintf(searchPath, sizeof(searchPath), "%s\\*.*", path);
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(searchPath, &fd);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
                 ObjString* str = copyString(fd.cFileName, (int)strlen(fd.cFileName));
                 push(&vm, OBJ_VAL(str));
                 // Append
                 if (list->capacity < list->count + 1) {
                     int oldCapacity = list->capacity;
                     list->capacity = GROW_CAPACITY(oldCapacity);
                     list->items = GROW_ARRAY(Value, list->items, oldCapacity, list->capacity);
                 }
                 list->items[list->count++] = OBJ_VAL(str);
                 pop(&vm);
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
#else
    DIR* d = opendir(path);
    if (d) {
        struct dirent* dir;
        while ((dir = readdir(d)) != NULL) {
             if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                 ObjString* str = copyString(dir->d_name, (int)strlen(dir->d_name));
                 push(&vm, OBJ_VAL(str));
                 // Append
                 if (list->capacity < list->count + 1) {
                     int oldCapacity = list->capacity;
                     list->capacity = GROW_CAPACITY(oldCapacity);
                     list->items = GROW_ARRAY(Value, list->items, oldCapacity, list->capacity);
                 }
                 list->items[list->count++] = OBJ_VAL(str);
                 pop(&vm);
             }
        }
        closedir(d);
    }
#endif

    pop(&vm); // list
    return OBJ_VAL(list);
}

// move(src, dest) -> Bool
static Value fs_move(int argCount, Value* args) {
    if (argCount < 2 || !IS_STRING(args[0]) || !IS_STRING(args[1])) return BOOL_VAL(false);
    return BOOL_VAL(rename(AS_CSTRING(args[0]), AS_CSTRING(args[1])) == 0);
}

// abspath(path) -> String or Null
static Value fs_abspath(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    const char* path = AS_CSTRING(args[0]);
    char resolved[4096]; // Max path buffer

#ifdef _WIN32
    if (_fullpath(resolved, path, 4096) != NULL) {
        return OBJ_VAL(copyString(resolved, (int)strlen(resolved)));
    }
#else
    if (realpath(path, resolved) != NULL) {
        return OBJ_VAL(copyString(resolved, (int)strlen(resolved)));
    }
#endif
    return NIL_VAL;
}

ObjModule* create_std_fs_module() {
    ObjString* name = copyString("std.native.fs", 13);
=======
ObjModule* create_std_fs_module() {
    ObjString* name = copyString("std.fs", 6);
>>>>>>> fix-ci-build
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
<<<<<<< HEAD
    defineModuleFn(module, "read_file", fs_read_file);
    defineModuleFn(module, "write_file", fs_write_file);
    defineModuleFn(module, "append_file", fs_append_file);
    defineModuleFn(module, "exists", fs_exists);
    defineModuleFn(module, "remove", fs_remove);
    defineModuleFn(module, "metadata", fs_metadata); 
    defineModuleFn(module, "mkdir", fs_mkdir);
    defineModuleFn(module, "rmdir", fs_rmdir);
    defineModuleFn(module, "listdir", fs_listdir);
    defineModuleFn(module, "is_file", fs_is_file);
    defineModuleFn(module, "is_dir", fs_is_dir);
    defineModuleFn(module, "copy", fs_copy);
    
    // New Functions
    defineModuleFn(module, "move", fs_move);
    defineModuleFn(module, "abspath", fs_abspath);
=======
    defineModuleFn(module, "readFile", fs_readFile);
    defineModuleFn(module, "writeFile", fs_writeFile);
    defineModuleFn(module, "exists", fs_exists);
>>>>>>> fix-ci-build
    
    pop(&vm);
    pop(&vm);
    return module;
}
