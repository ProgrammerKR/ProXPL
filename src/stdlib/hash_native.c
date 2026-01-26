// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-25
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/common.h"
#include "../../include/vm.h"
#include "../../include/value.h"
#include "../../include/object.h"
#include "../../include/md5.h"
#include "../../include/sha256.h"

extern VM vm;

// Helper
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// md5(str) -> String
static Value native_md5(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char *str = AS_CSTRING(args[0]);
    unsigned char digest[16];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, (const unsigned char*)str, strlen(str));
    MD5_Final(digest, &ctx);

    char buffer[33];
    for(int i = 0; i < 16; i++) {
        sprintf(&buffer[i*2], "%02x", digest[i]);
    }
    buffer[32] = '\0';
    
    return OBJ_VAL(copyString(buffer, 32));
}

// sha256(str) -> String
static Value native_sha256(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    
    const char *str = AS_CSTRING(args[0]);
    unsigned char digest[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const unsigned char*)str, strlen(str));
    sha256_final(&ctx, digest);

    char buffer[65];
    for(int i = 0; i < 32; i++) {
        sprintf(&buffer[i*2], "%02x", digest[i]);
    }
    buffer[64] = '\0';
    
    return OBJ_VAL(copyString(buffer, 64));
}

ObjModule* create_std_hash_module() {
    ObjString* name = copyString("std.native.hash", 15);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "md5", native_md5);
    defineModuleFn(module, "sha256", native_sha256);

    pop(&vm);
    pop(&vm);
    return module;
}
