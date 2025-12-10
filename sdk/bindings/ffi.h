#ifndef PROX_FFI_H
#define PROX_FFI_H

#include <stddef.h>

// ProX Value Types
typedef enum {
    PROX_NULL,
    PROX_INT,
    PROX_FLOAT,
    PROX_STRING
} ProxType;

typedef struct {
    ProxType type;
    union {
        long long i_val;
        double f_val;
        const char* s_val;
    } data;
} ProxValue;

// API
void prox_init();
ProxValue prox_call(const char* func_name, ProxValue* args, int argc);
void prox_register_native(const char* name, void* func_ptr);

#endif
