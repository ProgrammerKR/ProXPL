#include "ffi.h"
#include <stdio.h>

// Stub implementation of the FFI layer

void prox_init() { printf("[FFI] ProXPL Runtime Initialized\\n"); }

ProxValue prox_call(const char *func_name, ProxValue *args, int argc) {
  printf("[FFI] Calling function: %s with %d args\\n", func_name, argc);

  // Simulate return
  ProxValue ret;
  ret.type = PROX_INT;
  ret.data.i_val = 0;
  return ret;
}

void prox_register_native(const char *name, void *func_ptr) {
  printf("[FFI] Registered native function: %s\\n", name);
}
