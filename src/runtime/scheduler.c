// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-27
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include "../../include/object.h"
#include "../../include/value.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

// ----------------------------------------------------------------------------
// WORK-STEALING SCHEDULER (Chase-Lev Deque)
// ----------------------------------------------------------------------------

#define DEQUE_CAPACITY 1024
#define MAX_WORKERS 8

typedef struct {
    atomic_size_t top;
    atomic_size_t bottom;
    _Atomic(ObjTask*) buffer[DEQUE_CAPACITY];
} WorkerDeque;

// Global set of deques, one per worker (thread)
static WorkerDeque workers[MAX_WORKERS];
static atomic_int worker_count = 1; // Default to 1 (Main Thread) until we have thread pool

// Thread-Local ID
// In a real implementation with pthreads/WinThreads, this would be set on thread start.
// For now, we simulate with a global index since we run single-threaded in this environment.
// _Thread_local int thread_id = 0;
// We use a fallback macro for portability if C11 threads aren't fully set up
#if defined(__GNUC__) || defined(__clang__)
__thread int thread_id = 0;
#elif defined(_MSC_VER)
__declspec(thread) int thread_id = 0;
#else
int thread_id = 0; // Fallback for single thread
#endif

void scheduler_init(int worker_id) {
    thread_id = worker_id;
    atomic_init(&workers[worker_id].top, 0);
    atomic_init(&workers[worker_id].bottom, 0);
}

static void deque_push_bottom(WorkerDeque* deque, ObjTask* task) {
    size_t b = atomic_load_explicit(&deque->bottom, memory_order_relaxed);
    size_t t = atomic_load_explicit(&deque->top, memory_order_acquire);
    
    if (b - t > DEQUE_CAPACITY - 1) {
        // Queue full. For now, panic.
        // Real impl: Resize/Overflow buffer
        fprintf(stderr, "Scheduler Panic: Worker %d Deque Full!\n", thread_id);
        exit(1);
    }
    
    atomic_store_explicit(&deque->buffer[b % DEQUE_CAPACITY], task, memory_order_relaxed);
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&deque->bottom, b + 1, memory_order_relaxed);
}

static ObjTask* deque_take_bottom(WorkerDeque* deque) {
    size_t b = atomic_load_explicit(&deque->bottom, memory_order_relaxed) - 1;
    atomic_store_explicit(&deque->bottom, b, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);
    
    size_t t = atomic_load_explicit(&deque->top, memory_order_relaxed);
    
    if (t > b) {
        // Empty
        atomic_store_explicit(&deque->bottom, b + 1, memory_order_relaxed);
        return NULL;
    }
    
    ObjTask* task = atomic_load_explicit(&deque->buffer[b % DEQUE_CAPACITY], memory_order_relaxed);
    
    if (t == b) {
        // Single item, race against steal
        if (!atomic_compare_exchange_strong_explicit(&deque->top, &t, t + 1, 
                                                     memory_order_seq_cst, memory_order_relaxed)) {
            // Lost race
             atomic_store_explicit(&deque->bottom, b + 1, memory_order_relaxed);
             return NULL;
        }
        atomic_store_explicit(&deque->bottom, b + 1, memory_order_relaxed);
    }
    
    return task;
}

static ObjTask* deque_steal(WorkerDeque* deque) {
    size_t t = atomic_load_explicit(&deque->top, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    size_t b = atomic_load_explicit(&deque->bottom, memory_order_acquire);
    
    if (t >= b) return NULL;
    
    ObjTask* task = atomic_load_explicit(&deque->buffer[t % DEQUE_CAPACITY], memory_order_relaxed);
    
    if (!atomic_compare_exchange_strong_explicit(&deque->top, &t, t + 1,
                                                 memory_order_seq_cst, memory_order_relaxed)) {
        return NULL; // Failed to steal
    }
    
    return task;
}

// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------

static ObjTask* currentTask = NULL;

void scheduler_enqueue(ObjTask* task) {
    if (task->completed) return;
    deque_push_bottom(&workers[thread_id], task);
}

// Try to find a task: Local -> Global/Steal
static ObjTask* find_task() {
    // 1. Try Local
    ObjTask* task = deque_take_bottom(&workers[thread_id]);
    if (task) return task;
    
    // 2. Try Steal (Work Stealing)
    int count = atomic_load(&worker_count);
    for (int i = 0; i < count; i++) {
        if (i == thread_id) continue;
        task = deque_steal(&workers[i]);
        if (task) return task;
    }
    
    return NULL;
}

void scheduler_run() {
    while (1) {
        ObjTask* task = find_task();
        if (!task) break; // In real impl: park/sleep
        
        currentTask = task;
        // Resume
        if (task->coroHandle && task->resume) {
             task->resume(task->coroHandle);
        }
        currentTask = NULL;
        
        // If simulated single-thread, we might break if all queues empty
        // But for "google scale", we assume persistence.
        // Here we break if idle to return control to main (for testing/CLI)
        // Check if ALL queues empty?
        // For CLI 'run_and_wait', we just loop until done.
    }
}

// Runtime helpers called from LLVM

void prox_rt_await(Value taskVal) {
    if (!IS_TASK(taskVal)) {
        printf("Runtime Error: Awaiting non-task value.\n");
        exit(1);
    }
    ObjTask* target = AS_TASK(taskVal);
    
    if (target->completed) {
        if (currentTask) scheduler_enqueue(currentTask);
    } else {
        // In full impl: Add currentTask to target->waiters
        // For now: Re-enqueue (Busy-wait optimization)
        if (currentTask) scheduler_enqueue(currentTask);
    }
}

Value prox_rt_new_task(void* hdl, ResumeFn resume) {
    ObjTask* task = newTask(hdl, resume);
    scheduler_enqueue(task);
    return OBJ_VAL(task);
}

// Helper to bridge main synchronous code to async world
Value prox_rt_run_and_wait(Value taskVal) {
    if (!IS_TASK(taskVal)) {
        printf("Runtime Error: Awaiting non-task value in sync context.\n");
        exit(1);
    }
    ObjTask* task = AS_TASK(taskVal);
    scheduler_enqueue(task);
    
    // Run until this specific task is done
    // This is a blocking call for the main thread
    while (!task->completed) {
        ObjTask* work = find_task();
        if (work) {
            currentTask = work;
            if (work->coroHandle && work->resume) {
                work->resume(work->coroHandle);
            }
            currentTask = NULL;
        } else {
            // No work but task not done?
            // Deadlock or waiting on external IO
            // In stub, just break to avoid hang if logic is wrong
            // printf("DEADLOCK?\n"); 
            // break;
        }
    }
    return task->result;
}
