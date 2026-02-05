#include "asr_lockfree_queue.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

static ASRLockFreeQueue* g_queue = nullptr;
static std::atomic<bool> g_running{false};

void asr_supervisor_start(ASRLockFreeQueue* q) {
    g_queue = q;
    g_running.store(true);
    std::thread([](){
        using namespace std::chrono_literals;
        while (g_running.load()) {
            void* job = g_queue->pop();
            if (job) {
                // For demo: job pointer is actually a small integer cast
                intptr_t v = reinterpret_cast<intptr_t>(job);
                std::cout << "[supervisor] handling job: " << v << "\n";
            } else {
                std::this_thread::sleep_for(1ms);
            }
        }
    }).detach();
}

void asr_supervisor_stop() {
    g_running.store(false);
}
