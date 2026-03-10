// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pxcf.h"
#include "../include/object.h"
#include "../include/memory.h"

// Helper to check if a string represents a boolean
static bool isBoolLiteral(const char* str, bool* outVal) {
    if (strcmp(str, "true") == 0) {
        *outVal = true;
        return true;
    }
    if (strcmp(str, "false") == 0) {
        *outVal = false;
        return true;
    }
    return false;
}

// Helper to check if a string represents a number
static bool isNumberLiteral(const char* str, double* outVal) {
    char* endptr;
    *outVal = strtod(str, &endptr);
    // If strtod successfully converted the entire string (and it wasn't empty)
    return (*str != '\0' && *endptr == '\0');
}

// Helper to trim leading/trailing whitespace
static char* trimWhitespace(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

Value parsePxcf(VM* vm, const char* content) {
    // Create the root dictionary
    ObjDictionary* rootDict = newDictionary();
    push(vm, OBJ_VAL(rootDict)); // Protect from GC

    // We maintain a stack of active dictionaries to handle nesting
    // PXCF usually only has 1 level of nesting: root -> block
    ObjDictionary* activeDict = rootDict;
    
    // We'll duplicate the string to safely tokenize it line by line
    char* contentCopy = strdup(content);
    if (!contentCopy) {
        pop(vm);
        return NIL_VAL;
    }

    // Tokenize by newline
    char* line = strtok(contentCopy, "\n\r");
    while (line != NULL) {
        char* trimmed = trimWhitespace(line);
        
        // Skip empty lines and comments
        if (trimmed[0] == '\0' || trimmed[0] == '#') {
            line = strtok(NULL, "\n\r");
            continue;
        }
        
        // Check for block closing
        if (trimmed[0] == '}') {
            // Restore activeDict to root
            activeDict = rootDict;
            line = strtok(NULL, "\n\r");
            continue;
        }

        // Check for block opening e.g., "project {"
        char* openBrace = strchr(trimmed, '{');
        if (openBrace != NULL) {
            *openBrace = '\0'; // Null-terminate before the brace
            char* blockName = trimWhitespace(trimmed);
            
            ObjDictionary* newBlock = newDictionary();
            push(vm, OBJ_VAL(newBlock));
            
            ObjString* keyStr = copyString(blockName, (int)strlen(blockName));
            push(vm, OBJ_VAL(keyStr));
            
            tableSet(&rootDict->items, keyStr, OBJ_VAL(newBlock));
            
            pop(vm); // pop keyStr
            pop(vm); // pop newBlock
            
            activeDict = newBlock;
            line = strtok(NULL, "\n\r");
            continue;
        }

        // Check for key: value pair
        char* colonStr = strchr(trimmed, ':');
        if (colonStr != NULL) {
            *colonStr = '\0'; // Split into key and value
            char* rawKey = trimWhitespace(trimmed);
            char* rawVal = trimWhitespace(colonStr + 1);

            // Create key object
            ObjString* keyObj = copyString(rawKey, (int)strlen(rawKey));
            push(vm, OBJ_VAL(keyObj));
            
            Value valObj = NIL_VAL;

            // Determine value type
            if (rawVal[0] == '"') {
                char* endQuote = strrchr(rawVal, '"');
                if (endQuote && endQuote != rawVal) {
                    *endQuote = '\0';
                    char* strContent = rawVal + 1;
                    valObj = OBJ_VAL(copyString(strContent, (int)strlen(strContent)));
                } else {
                     // Malformed string, treat as string up to end
                    char* strContent = rawVal + 1;
                    valObj = OBJ_VAL(copyString(strContent, (int)strlen(strContent)));
                }
            } else {
                bool bVal;
                double dVal;
                if (isBoolLiteral(rawVal, &bVal)) {
                    valObj = BOOL_VAL(bVal);
                } else if (isNumberLiteral(rawVal, &dVal)) {
                    valObj = NUMBER_VAL(dVal);
                } else {
                    // Fallback to storing as unquoted string
                    valObj = OBJ_VAL(copyString(rawVal, (int)strlen(rawVal)));
                }
            }

            push(vm, valObj);
            tableSet(&activeDict->items, keyObj, valObj);
            pop(vm); // pop valObj
            pop(vm); // pop keyObj
        }

        line = strtok(NULL, "\n\r");
    }

    free(contentCopy);
    
    // Return the root dictionary
    Value result = peek(vm, 0); 
    pop(vm); // pop rootDict
    return result;
}

static char* readFileContent(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}


Value nativeLoadConfig(int argCount, Value *args) {
    if (argCount != 1 || !IS_STRING(args[0])) {
         // Should realistically raise an error, but returning nil is safer fallback natively
        return NIL_VAL;
    }
    
    ObjString* pathStr = AS_STRING(args[0]);
    char* content = readFileContent(pathStr->chars);
    
    if (content == NULL) {
        return NIL_VAL;
    }
    
    // Externally visible state: loadConfig returns a new Dictionary
    // Need access to current VM thread. Since native arguments do not inherently
    // carry VM in standard signature, but we are inside the engine...
    // ProXPL globals usually rely on a global `vm` or we pass it?
    // Wait, ProXPL standard library C architecture requires standard VM.
    // However, some engines stash the current VM. We'll use a hack or if it's not feasible,
    // we need to see how ProXPL natives are defined.
    // Wait, let's look at `native_len`. It doesn't take VM.
    // If we need to allocate Dictionary, `newDictionary` might use a global memory allocator or require vm?
    
    // In `object.h`: `struct ObjDictionary *newDictionary();`
    // It seems ProXPL natively allocates objects using implicit global VM state or 
    // `newDictionary()` handles GC registering without explicit VM passed!
    
    // Wait, `parsePxcf` requires `VM* vm` for push/pop GC protection. 
    // We can just define `extern VM vm;` since it's a global in ProXPL (often `extern VM vm;` in `vm.h`).
    
    extern VM vm;
    Value result = parsePxcf(&vm, content);
    
    free(content);
    return result;
}
