/*
 * ProXPL Standard Library - System Module
 * Native C implementation of system functions
 */

#include "../include/common.h"
#include "../include/vm.h"
#include "../include/value.h"
#include "../include/object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    return MAKE_NULL(); // Never reached
}

// env(name) - Get environment variable
static Value native_env(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) {
        return MAKE_NULL();
    }
    
    const char* name = AS_CSTRING(args[0]);
    const char* value = getenv(name);
    
    if (value) {
        return MAKE_OBJ(copyString(value, strlen(value)));
    }
    return MAKE_NULL();
}

// platform() - Get platform name
static Value native_platform(int argCount, Value* args) {
#ifdef _WIN32
    return MAKE_OBJ(copyString("Windows", 7));
#elif __APPLE__
    return MAKE_OBJ(copyString("macOS", 5));
#elif __linux__
    return MAKE_OBJ(copyString("Linux", 5));
#else
    return MAKE_OBJ(copyString("Unknown", 7));
#endif
}

// version() - Get ProXPL version
static Value native_version(int argCount, Value* args) {
    return MAKE_OBJ(copyString("ProXPL 1.0.0", 12));
}

// exec(command) - Execute shell command
static Value native_exec(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) {
        return MAKE_NULL();
    }
    
    const char* command = AS_CSTRING(args[0]);
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        return MAKE_NULL();
    }
    
    char buffer[4096];
    char* result = (char*)malloc(1);
    result[0] = '\0';
    size_t totalLen = 0;
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t len = strlen(buffer);
        result = (char*)realloc(result, totalLen + len + 1);
        strcpy(result + totalLen, buffer);
        totalLen += len;
    }
    
    pclose(pipe);
    
    Value output = MAKE_OBJ(copyString(result, totalLen));
    free(result);
    return output;
}

// time() - Get current timestamp in seconds since epoch
static Value native_time(int argCount, Value* args) {
    time_t now = time(NULL);
    return MAKE_NUMBER((double)now);
}

// sleep(seconds) - Sleep for specified seconds  
static Value native_sleep(int argCount, Value* args) {
    if (argCount < 1 || !IS_NUMBER(args[0])) {
        return MAKE_NULL();
    }
    
    int seconds = (int)AS_NUMBER(args[0]);
    
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
    
    return MAKE_NULL();
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
