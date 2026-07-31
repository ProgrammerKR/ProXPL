#include <iostream>
#include <chrono>

int collatz(int n) {
    int steps = 0;
    while(n > 1) {
        if(n % 2 == 0) n /= 2;
        else n = 3 * n + 1;
        steps++;
    }
    return steps;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    int max_steps = 0;
    for(int i = 1; i <= 20000; i++) {
        int steps = collatz(i);
        if(steps > max_steps) max_steps = steps;
    }
    std::cout << "Max Collatz steps: " << max_steps << "\n";
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << elapsed.count() << "\n";
    return 0;
}
