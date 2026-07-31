#include <iostream>
#include <vector>
#include <chrono>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> arr;
    for(int i=0; i<50000; i++) arr.push_back(i);
    
    long long total_sum = 0;
    for(size_t i=0; i<arr.size(); i++) total_sum += arr[i];
    
    for(int i=0; i<25000; i++) arr.pop_back();
    
    std::cout << "Array Sum: " << total_sum << "\n";
    std::cout << "Array Length: " << arr.size() << "\n";
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << elapsed.count() << "\n";
    return 0;
}
