// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

// supervisor.c - Autonomic Supervisor for ProXPL Runtime
// Responsible for monitoring execution, handling panics, and executing recovery strategies.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../include/object.h"
#include "../../include/value.h"

// ----------------------------------------------------------------------------
// FAULT TOLERANCE: SUPERVISOR TREES (Erlang/OTP Style)
// ----------------------------------------------------------------------------

typedef enum {
    STRATEGY_ONE_FOR_ONE,
    STRATEGY_ALL_FOR_ONE,
    STRATEGY_REST_FOR_ONE
} RestartStrategy;

typedef struct ChildSpec {
    int id;
    ObjTask* task; // The monitored task
    // ObjSupervisor* sub_supervisor; // Recursive
    int max_retries;
    int current_retries;
    struct ChildSpec* next;
} ChildSpec;

typedef struct Supervisor {
    int id;
    RestartStrategy strategy;
    ChildSpec* children; // Linked list of children
    struct Supervisor* parent; // Up-link for escalation
} Supervisor;

// Global Root Supervisor
static Supervisor rootSupervisor;
static bool initialized = false;

void initSupervisor() {
    rootSupervisor.id = 0;
    rootSupervisor.strategy = STRATEGY_ONE_FOR_ONE;
    rootSupervisor.children = NULL;
    rootSupervisor.parent = NULL;
    initialized = true;
    printf("[Supervisor] Initialized Autonomic Self-Healing Subsystem.\n");
}

static ChildSpec* find_child(Supervisor* sup, int task_id) {
    ChildSpec* curr = sup->children;
    while (curr) {
        if (curr->id == task_id) return curr;
        curr = curr->next;
    }
    return NULL;
}

void registerTask(int taskId, ObjTask* task, int maxRetries) {
    if (!initialized) initSupervisor();
    
    // In full impl, we'd specify which supervisor to attach to.
    // Default to Root.
    
    ChildSpec* child = malloc(sizeof(ChildSpec));
    child->id = taskId;
    child->task = task;
    child->max_retries = maxRetries;
    child->current_retries = 0;
    child->next = rootSupervisor.children;
    rootSupervisor.children = child;
    
    printf("[Supervisor] Monitoring Task %d (Retries: %d)\n", taskId, maxRetries);
}

// Restart a specific child logic (Stub)
static void restart_child(ChildSpec* child) {
    printf("[Supervisor] RESTARTING Child %d...\n", child->id);
    // In real VM:
    // 1. Reset Task IP/Stack
    // 2. Scheduler Enqueue(child->task)
    child->current_retries++;
    // Stub:
    // child->task->completed = false;
    // scheduler_enqueue(child->task);
}

static void handle_failure(Supervisor* sup, ChildSpec* failedChild) {
    printf("[Supervisor] Handling Failure for Child %d using Strategy %d\n", 
           failedChild->id, sup->strategy);
           
    if (failedChild->current_retries >= failedChild->max_retries) {
        printf("[Supervisor] Child %d exceeded max retries (%d). ESCALATING.\n", 
               failedChild->id, failedChild->max_retries);
        // Escalate to parent
        if (sup->parent) {
             // propagate panic up
        } else {
             printf("[Supervisor] Root Supervisor Gave Up. SYSTEM CRASH.\n");
             exit(1);
        }
        return;
    }

    switch (sup->strategy) {
        case STRATEGY_ONE_FOR_ONE:
            restart_child(failedChild);
            break;
            
        case STRATEGY_ALL_FOR_ONE:
            // Restart ALL children
            {
                ChildSpec* curr = sup->children;
                while (curr) {
                    restart_child(curr);
                    curr = curr->next;
                }
            }
            break;
            
        default:
            break;
    }
}

// Hook called by VM exception handler
void notifyPanic(int taskId, const char *message) {
    if (!initialized) {
         printf("Panic before supervisor init: %s\n", message);
         exit(1);
    }
    
    printf("[Supervisor] ALERT: Task %d Panicked! Msg: %s\n", taskId, message);
    
    // Find who owns this task
    // Simplified: Search root
    ChildSpec* match = find_child(&rootSupervisor, taskId);
    if (match) {
        handle_failure(&rootSupervisor, match);
    } else {
        printf("[Supervisor] Unsupervised Task %d crashed. Ignoring.\n", taskId);
    }
}

// Compatibility shim for existing calls
void registerResilientBlock(int id, const char *strategyStr, int retryCount) {
    // Map old API to new task registration
    // We assume ID maps to a task somehow or creates a dummy task wrapper
    registerTask(id, NULL, retryCount);
}
