// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

/*
 * ProXPL Standard Library - System Module
 * Native C implementation of system functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Required for time()

#include "common.h"
#include "vm.h"
#include "value.h"
#include "object.h"

#ifdef _WIN32
  #include <windows.h>
  #define popen _popen
  #define pclose _pclose
#else
  #include <unistd.h>
#endif

// exit(code) - Exit program
static Value native_exit(int argCount, Value* args) {
    int code = 0;
    if (argCount > 0 && IS_NUMBER(args[0])) {
        code = (int)AS_NUMBER(args[0]);
    }
    exit(code);
    return NIL_VAL; // Never reached, but satisfies compiler
}

// env(name) - Get environment variable
static Value native_env(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) {
        return NIL_VAL;
    }
    
    const char* name = AS_CSTRING(args[0]);
    const char* value = getenv(name);
    
    if (value) {
        return OBJ_VAL(copyString(value, (int)strlen(value)));
    }
    return NIL_VAL;
}

// platform() - Get platform name
static Value native_platform(int argCount, Value* args) {
    (void)argCount; (void)args;
#ifdef _WIN32
    return OBJ_VAL(copyString("Windows", 7));
#elif __APPLE__
    return OBJ_VAL(copyString("macOS", 5));
#elif __linux__
    return OBJ_VAL(copyString("Linux", 5));
#else
    return OBJ_VAL(copyString("Unknown", 7));
#endif
}

// version() - Get ProXPL version
static Value native_version(int argCount, Value* args) {
    (void)argCount; (void)args;
    return OBJ_VAL(copyString("ProXPL 1.0.0", 12));
}

// exec(command) - Execute shell command
static Value native_exec(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) {
        return NIL_VAL;
    }
    
    const char* command = AS_CSTRING(args[0]);
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        return NIL_VAL;
    }
    
    char buffer[4096];
    char* result = (char*)malloc(1);
    if (!result) {
        pclose(pipe);
        return NIL_VAL;
    }
    result[0] = '\0';
    size_t totalLen = 0;
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t len = strlen(buffer);
        char* temp = (char*)realloc(result, totalLen + len + 1);
        if (!temp) {
            free(result);
            pclose(pipe);
            return NIL_VAL;
        }
        result = temp;
        strcpy(result + totalLen, buffer);
        totalLen += len;
    }
    
    pclose(pipe);
    
    Value output = OBJ_VAL(copyString(result, (int)totalLen));
    free(result);
    return output;
}

// time() - Get current timestamp in seconds since epoch
static Value native_time(int argCount, Value* args) {
    (void)argCount; (void)args;
    time_t now = time(NULL);
    return NUMBER_VAL((double)now);
}

// sleep(seconds) - Sleep for specified seconds  
static Value native_sleep(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) {
        return NIL_VAL;
    }
    
    int seconds = (int)AS_NUMBER(args[0]);
    
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
    
    return NIL_VAL;
}

// Register all system functions with the VM
void register_system_natives(VM* vm) {
    defineNative(vm, "exit", native_exit);
    defineNative(vm, "env", native_env);
    defineNative(vm, "platform", native_platform);
    defineNative(vm, "version", native_version);
    defineNative(vm, "exec", native_exec);
    defineNative(vm, "time", native_time);
    defineNative(vm, "sleep", native_sleep);
}

