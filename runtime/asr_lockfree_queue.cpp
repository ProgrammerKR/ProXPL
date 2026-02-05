#include "asr_lockfree_queue.h"
#include <cassert>
#include <thread>

ASRLockFreeQueue::ASRLockFreeQueue(size_t capacity)
    : cap((capacity < 2) ? 2 : capacity), buffer(cap), head(0), tail(0) {}

ASRLockFreeQueue::~ASRLockFreeQueue() {}

bool ASRLockFreeQueue::push(void* item) {
    uint64_t t = tail.load(std::memory_order_relaxed);
    for (;;) {
        uint64_t h = head.load(std::memory_order_acquire);
        if ((t - h) >= cap) return false; // full
        if (tail.compare_exchange_weak(t, t + 1, std::memory_order_acq_rel)) {
            size_t idx = (t) % cap;
            buffer[idx] = item;
            return true;
        }
        std::this_thread::yield();
    }
}

void* ASRLockFreeQueue::pop() {
    uint64_t h = head.load(std::memory_order_relaxed);
    for (;;) {
        uint64_t t = tail.load(std::memory_order_acquire);
        if (h == t) return nullptr; // empty
        if (head.compare_exchange_weak(h, h + 1, std::memory_order_acq_rel)) {
            size_t idx = (h) % cap;
            void* item = buffer[idx];
            buffer[idx] = nullptr;
            return item;
        }
        std::this_thread::yield();
    }
}
