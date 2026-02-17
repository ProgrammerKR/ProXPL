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

// ----------------------------------------------------------------------------
// NATIVE NETWORKING: ASYNC I/O (IOCP/Epoll Simulation)
// ----------------------------------------------------------------------------

extern VM vm;

// Mock Handle for socket
typedef struct {
    int id;
} SocketHandle;

static int socket_counter = 1;

// Helper to define native function in a module
static void defineModuleFn(ObjModule* module, const char* name, NativeFn function) {
    ObjString* nameObj = copyString(name, (int)strlen(name));
    push(&vm, OBJ_VAL(nameObj));
    push(&vm, OBJ_VAL(newNative(function)));
    tableSet(&module->exports, nameObj, peek(&vm, 0));
    pop(&vm);
    pop(&vm);
}

// In a real implementation, these would interact with the OS and the Scheduler.
// Since we don't have the full Event Loop in this MVP, we simulate "Async" behavior
// by returning a completed Task or a mock "Promise".

// net.tcp_listener(addr) -> Listener
static Value native_tcp_listener(int argCount, Value* args) {
    if (argCount < 1 || !IS_STRING(args[0])) return NIL_VAL;
    const char* addr = AS_CSTRING(args[0]);
    printf("[Net] Binding TCP Listener to %s (fd: %d)\n", addr, socket_counter);
    // Return a dummy "Listener" object (using Foreign/Native for now or just an ID)
    // We'll reuse ObjForeign to hold the handle.
    return OBJ_VAL(newForeign(copyString("TCPListener", 11), (void*)(intptr_t)socket_counter++, NULL));
}

// net.accept(listener) -> Task<Socket>
static Value native_accept(int argCount, Value* args) {
    if (argCount < 1 || !IS_FOREIGN(args[0])) return NIL_VAL;
    
    // In real Async/Await:
    // 1. Create a Promise/Task.
    // 2. Register callback with IOCP/Epoll.
    // 3. Return the Task.
    // 4. (Later) Scheduler wakes up Task when connection arrives.
    
    printf("[Net] Async Accept... (Simulated Immediate Success)\n");
    
    // Mock new connection
    ObjForeign* conn = newForeign(copyString("TCPSocket", 9), (void*)(intptr_t)socket_counter++, NULL);
    
    // We return a "Task" that is already completed for MDV/Stub purposes
    // Or we should allow the VM to construct a Task wrapping this value?
    // Let's assume idiomatic ProXPL: `let conn = await server.accept()`
    // This native fn should return a Task object.
    
    ObjTask* task = newTask(NULL, NULL);
    task->completed = true;
    task->result = OBJ_VAL(conn);
    return OBJ_VAL(task);
}

// net.read(socket) -> Task<String>
static Value native_read(int argCount, Value* args) {
    // printf("[Net] Async Read...\n");
    ObjTask* task = newTask(NULL, NULL);
    task->completed = true;
    task->result = OBJ_VAL(copyString("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", 36));
    return OBJ_VAL(task);
}

// net.write(socket, data) -> Task<Void>
static Value native_write(int argCount, Value* args) {
    if (argCount < 2) return NIL_VAL;
    // printf("[Net] Async Write: %s\n", AS_CSTRING(args[1]));
    
    ObjTask* task = newTask(NULL, NULL);
    task->completed = true;
    task->result = NIL_VAL;
    return OBJ_VAL(task);
}

ObjModule* create_std_net_module() {
    ObjString* name = copyString("std.native.net", 14);
    push(&vm, OBJ_VAL(name));
    ObjModule* module = newModule(name);
    push(&vm, OBJ_VAL(module));
    
    defineModuleFn(module, "tcp_listener", native_tcp_listener);
    defineModuleFn(module, "accept", native_accept);
    defineModuleFn(module, "read", native_read);
    defineModuleFn(module, "write", native_write);

    pop(&vm);
    pop(&vm);
    return module;
}
