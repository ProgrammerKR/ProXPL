// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Module:  PXCF Integration Bridge
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pxcf/pxcf.h"
#include "../include/object.h"
#include "../include/memory.h"
#include "../include/vm.h"

// The global VM instance used by ProXPL
extern VM vm;

static Value pxcfValueToProxValue(PxcfValue* pval) {
    if (!pval) return NIL_VAL;
    
    PxcfValueType type = pxcf_value_type(pval);
    switch (type) {
        case PXCF_VALUE_NULL: {
            return NIL_VAL;
        }
        case PXCF_VALUE_BOOL: {
            bool b = false;
            pxcf_value_get_bool(pval, &b);
            return BOOL_VAL(b);
        }
        case PXCF_VALUE_INTEGER: {
            int64_t i = 0;
            pxcf_value_get_integer(pval, &i);
            return NUMBER_VAL((double)i);
        }
        case PXCF_VALUE_FLOAT: {
            double f = 0.0;
            pxcf_value_get_float(pval, &f);
            return NUMBER_VAL(f);
        }
        case PXCF_VALUE_STRING: {
            size_t len = 0;
            const char* str = pxcf_value_get_string(pval, &len);
            if (!str) return NIL_VAL;
            return OBJ_VAL(copyString(str, (int)len));
        }
        case PXCF_VALUE_ARRAY: {
            ObjList* list = newList();
            push(&vm, OBJ_VAL(list));
            size_t size = pxcf_array_size(pval);
            for (size_t i = 0; i < size; i++) {
                PxcfValue* item = pxcf_array_get(pval, i);
                Value valObj = pxcfValueToProxValue(item);
                push(&vm, valObj);
                appendToList(list, valObj);
                pop(&vm); // valObj
            }
            pop(&vm); // list
            return OBJ_VAL(list);
        }
        case PXCF_VALUE_OBJECT: {
            ObjDictionary* dict = newDictionary();
            push(&vm, OBJ_VAL(dict));
            size_t size = pxcf_object_size(pval);
            for (size_t i = 0; i < size; i++) {
                const char* key = NULL;
                PxcfValue* item = pxcf_object_get_index(pval, i, &key);
                if (!key || !item) continue;
                
                ObjString* keyObj = copyString(key, (int)strlen(key));
                push(&vm, OBJ_VAL(keyObj));
                
                Value valObj = pxcfValueToProxValue(item);
                push(&vm, valObj);
                
                tableSet(&dict->items, keyObj, valObj);
                
                pop(&vm); // valObj
                pop(&vm); // keyObj
            }
            pop(&vm); // dict
            return OBJ_VAL(dict);
        }
    }
    return NIL_VAL;
}

Value nativeLoadConfig(int argCount, Value *args) {
    if (argCount != 1 || !IS_STRING(args[0])) {
        return NIL_VAL;
    }
    
    ObjString* pathStr = AS_STRING(args[0]);
    
    PxcfError err;
    PxcfDocument* doc = pxcf_load_file(pathStr->chars, &err);
    if (!doc) {
        return NIL_VAL;
    }
    
    PxcfValue* root = pxcf_document_root(doc);
    Value result = pxcfValueToProxValue(root);
    
    pxcf_document_free(doc);
    return result;
}
