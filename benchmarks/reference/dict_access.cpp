#include <iostream>
#include <unordered_map>
#include <string>
#include <chrono>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::unordered_map<std::string, int> d;
    for(int i=0; i<10000; i++) {
        d["key" + std::to_string(i)] = i;
    }
    
    long long total_sum = 0;
    for(int i=0; i<10000; i++) {
        total_sum += d["key" + std::to_string(i)];
    }
    
    std::cout << "Dict Sum: " << total_sum << "\n";
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << elapsed.count() << "\n";
    return 0;
}
