// asr_runtime.h -- minimal ASR runtime ABI (sketch)
#pragma once

#include <cstdint>

typedef void (*asr_handler_fn)(void* obj, void* ctx);

extern "C" {
  // Register a handler for a given type-id (language runtime provides stable ids)
  void asr_register_handler(uint64_t type_id, asr_handler_fn handler);

  // Called from compiler-emitted checkpoint probes with a pointer to thread-local state
  void asr_checkpoint(void* thread_state);

  // Report anomalies (used by fast-paths to escalate to supervisor)
  void asr_report_anomaly(uint32_t code, void* data);
}

// Platform-specific helpers (implementation file)
// - lock-free queues
// - supervisor thread initialization
