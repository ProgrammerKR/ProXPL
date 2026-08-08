#include "../runtime/asr_runtime_full.h"
#include "../runtime/asr_lockfree_queue.h"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
    // Start supervisor
    asr_supervisor_start();

    // Push jobs with different priorities using the public API
    for (int i = 0; i < 5; ++i) {
        intptr_t job = 100 + i;
        asr_report_anomaly(1, reinterpret_cast<void*>(job), ASRPriority::Critical);
    }
    for (int i = 0; i < 5; ++i) {
        intptr_t job = 200 + i;
        asr_report_anomaly(1, reinterpret_cast<void*>(job), ASRPriority::UserVisible);
    }
    for (int i = 0; i < 5; ++i) {
        intptr_t job = 300 + i;
        asr_report_anomaly(1, reinterpret_cast<void*>(job), ASRPriority::Background);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    asr_supervisor_stop();
    std::cout << "ASR runtime test complete\n";
    return 0;
}
