#pragma once
#include <cstdint>
#include <functional>

// ASR runtime (full sketch) -- prioritized repair buckets and runtime API

enum class ASRPriority : int { Critical = 0, UserVisible = 1, Background = 2 };

typedef void (*asr_handler_fn)(void* obj, void* ctx);

extern "C" {
  // Register a handler for a type id
  void asr_register_handler(uint64_t type_id, asr_handler_fn handler);

  // Compiler emits checkpoints: fast inline check into asr_checkpoint
  void asr_checkpoint(void* thread_state);

  // Report an anomaly and enqueue a repair job with a priority
  void asr_report_anomaly(uint32_t code, void* data, ASRPriority priority);

  // Start/stop supervisor (for embedding in runtime)
  void asr_supervisor_start();
  void asr_supervisor_stop();
}
