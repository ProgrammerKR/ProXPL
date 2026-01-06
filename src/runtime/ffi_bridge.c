#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/common.h"
#include "../include/object.h"
#include "../include/value.h"
#include "../include/ffi_bridge.h"

// Platform-specific includes for dynamic loading
#ifdef _WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

#ifdef USE_LIBFFI
  #include <ffi.h>
#endif

ObjForeign* loadForeign(ObjString* libraryPath, ObjString* symbolName) {
    void* handle = NULL;
    void* symbol = NULL;

    const char* libName = libraryPath ? libraryPath->chars : NULL;
    const char* symName = symbolName->chars;

#ifdef _WIN32
    if (libName) {
        handle = (void*)LoadLibraryA(libName);
    } else {
        handle = (void*)GetModuleHandle(NULL);
    }
    
    if (!handle) {
        // Failed to load library
        return NULL;
    }

    symbol = (void*)GetProcAddress((HMODULE)handle, symName);
#else
    if (libName) {
        handle = dlopen(libName, RTLD_LAZY | RTLD_GLOBAL);
    } else {
        handle = dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL);
    }

    if (!handle) {
        return NULL;
    }

    symbol = dlsym(handle, symName);
#endif

    if (!symbol) {
        // Symbol not found. 
        // We might want to close handle if we opened it, but for now we assume 
        // libraries might be reused or it's fine to leak handle until exit.
        return NULL;
    }

    return newForeign(symbolName, handle, symbol);
}

Value callForeign(ObjForeign* foreign, int argCount, Value* args) {
#ifndef USE_LIBFFI
    printf("FFI not enabled in this build.\n");
    return NIL_VAL;
#else
    if (!foreign->function) return NIL_VAL;

    ffi_cif cif;
    ffi_type** argTypes = (ffi_type**)malloc(sizeof(ffi_type*) * argCount);
    void** argValues = (void**)malloc(sizeof(void*) * argCount);
    
    // We need storage for the raw values passed to ffi_call
    // union to handle different types
    typedef union {
        int i;
        double d;
        char* p;
        long long l;
    } ArgStorage;
    
    ArgStorage* storage = (ArgStorage*)malloc(sizeof(ArgStorage) * argCount);

    for (int i = 0; i < argCount; i++) {
        Value val = args[i];
        if (IS_NUMBER(val)) {
            // Check if it's an integer or strict float? 
            // ProXPL uses double for everything.
            // If we want to support C ints, we might auto-detect?
            // "Infer" strategy:
            // If it has no fractional part, pass as int?
            // No, that's dangerous. 'puts' expects char*, 'abs' expects int.
            // BUT 'sin' expects double.
            // Safe bet for C FFI without signatures:
            // We really need signatures.
            // Plan said: "Infer", but also:
            // "Number -> double"
            // "String -> char*"
            // "Bool -> int"
            
            // So we stick to double. C int functions might receive garbage if they read 64-bit double as int.
            // WAIT. `puts` takes `const char*`.
            // `abs` takes `int`.
            // Calling `abs(double)` via FFI expecting `int` will fail/crash.
            // C ABI is strict.
            // We MUST support int.
            // Let's check if the double value is exactly an integer.
            double n = AS_NUMBER(val);
            if ((double)(int)n == n) {
               // It's an integer. Pass as int.
               // This is heuristically better for typical C APIs.
               argTypes[i] = &ffi_type_sint;
               storage[i].i = (int)n;
               argValues[i] = &storage[i].i;
            } else {
               argTypes[i] = &ffi_type_double;
               storage[i].d = n;
               argValues[i] = &storage[i].d;
            }
        } else if (IS_STRING(val)) {
            argTypes[i] = &ffi_type_pointer;
            storage[i].p = AS_CSTRING(val);
            argValues[i] = &storage[i].p;
        } else if (IS_BOOL(val)) {
            argTypes[i] = &ffi_type_sint;
            storage[i].i = AS_BOOL(val) ? 1 : 0;
            argValues[i] = &storage[i].i;
        } else if (IS_NULL(val)) {
            argTypes[i] = &ffi_type_pointer;
            storage[i].p = NULL;
            argValues[i] = &storage[i].p;
        } else {
            // Pass pointer to object handle
            argTypes[i] = &ffi_type_pointer;
            storage[i].p = (void*)AS_OBJ(val);
            argValues[i] = &storage[i].p;
        }
    }

    // Default return type: int (most common)
    // Ideally we'd support double too.
    // Let's use ffi_type_pointer sized return buffer and cast?
    // ffi_type_sint is platform int.
    
    // We'll prepare for an 'int' return for now as 'puts' returns int.
    // To support double return, we'd need syntax override.
    ffi_status status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argCount, &ffi_type_pointer, argTypes);
    
    // Using pointer-sized return buffer to catch int/ptr.
    // If it returns double, we might read garbage.
    void* resultPtr = NULL; // storage for return
    
    // Actually, ffi_call writes result to the pointer provided.
    // It must effectively be sizeof(return_type).
    // Let's use a generic large buffer.
    long long retStorage = 0;
    
    // Temporarily forcing int return for `puts`.
    // ffi_type_sint
    status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argCount, &ffi_type_sint, argTypes);
    
    if (status == FFI_OK) {
        ffi_call(&cif, FFI_FN(foreign->function), &retStorage, argValues);
        
        free(argTypes);
        free(argValues);
        free(storage);
        
        return NUMBER_VAL((double)(int)retStorage);
    }
    
    free(argTypes);
    free(argValues);
    free(storage);
    return NIL_VAL;
#endif
}
