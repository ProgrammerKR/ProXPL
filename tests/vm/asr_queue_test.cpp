#include "../runtime/asr_lockfree_queue.h"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
    ASRLockFreeQueue q(1024);
    // start supervisor
    extern void asr_supervisor_start(ASRLockFreeQueue*);
    extern void asr_supervisor_stop();
    asr_supervisor_start(&q);

    // push some jobs
    for (int i = 1; i <= 20; ++i) {
        intptr_t v = i;
        while (!q.push(reinterpret_cast<void*>(v))) {
            std::this_thread::yield();
        }
    }

    // allow supervisor to consume
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    asr_supervisor_stop();
    std::cout << "Test complete\n";
    return 0;
}
