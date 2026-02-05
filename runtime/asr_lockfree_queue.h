#pragma once
#include <atomic>
#include <cstdint>
#include <vector>

// Simple bounded MPMC ring buffer for pointer-sized job handles.
// This is an illustrative sketch, not a production-quality queue.
class ASRLockFreeQueue {
public:
    explicit ASRLockFreeQueue(size_t capacity);
    ~ASRLockFreeQueue();

    // Push a void* job. Returns false if queue full.
    bool push(void* item);

    // Pop a void* job. Returns nullptr if queue empty.
    void* pop();

private:
    const size_t cap;
    std::vector<void*> buffer;
    std::atomic<uint64_t> head; // consumer index
    std::atomic<uint64_t> tail; // producer index
    // tail and head carry sequence counters; index = seq % cap
};
