// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-27
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "../../include/object.h"
#include "../../include/value.h"
#include <stdio.h>
#include <stdlib.h>

// Extern declaration of generated LLVM wrapper
extern void prox_rt_resume(void* hdl);

typedef struct TaskQueue {
    ObjTask* head;
    ObjTask* tail;
} TaskQueue;

static TaskQueue queue = {NULL, NULL};
static ObjTask* currentTask = NULL;

void scheduler_enqueue(ObjTask* task) {
    if (task->completed) return;
    
    task->next = NULL;
    if (queue.tail) {
        queue.tail->next = task;
        queue.tail = task;
    } else {
        queue.head = task;
        queue.tail = task;
    }
}

ObjTask* scheduler_dequeue() {
    if (!queue.head) return NULL;
    ObjTask* task = queue.head;
    queue.head = task->next;
    if (!queue.head) queue.tail = NULL;
    return task;
}

void scheduler_run() {
    while (queue.head) {
        ObjTask* task = scheduler_dequeue();
        currentTask = task;
        // Resume
        if (task->coroHandle) {
             prox_rt_resume(task->coroHandle);
        }
        currentTask = NULL;
    }
}

// Runtime helpers called from LLVM

void prox_rt_await(Value taskVal) {
    if (!IS_TASK(taskVal)) {
        printf("Runtime Error: Awaiting non-task value.\\n");
        exit(1);
    }
    ObjTask* target = AS_TASK(taskVal);
    
    if (target->completed) {
        // Optimization: If done, don't suspend?
        // But we already emitted suspend code unconditionally in backend.
        // So we MUST re-schedule current task immediately.
        if (currentTask) scheduler_enqueue(currentTask);
    } else {
        // Dependency: currentTask depends on target.
        // For simplicity in this demo, we just re-enqueue currentTask.
        // A real scheduler would put currentTask in target's "waiters" list.
        // Busy-wait behavior via re-enqueueing is inefficient but functional for MVP.
        if (currentTask) scheduler_enqueue(currentTask);
    }
}

Value prox_rt_new_task(void* hdl) {
    ObjTask* task = newTask(hdl);
    // Auto-schedule new tasks? 
    // Yes, usually.
    scheduler_enqueue(task);
    return OBJ_VAL(task);
}

// Helper to bridge main synchronous code to async world
Value prox_rt_run_and_wait(Value taskVal) {
    if (!IS_TASK(taskVal)) {
        printf("Runtime Error: Awaiting non-task value in sync context.\\n");
        exit(1);
    }
    ObjTask* task = AS_TASK(taskVal);
    scheduler_enqueue(task);
    scheduler_run();
    return task->result;
}
