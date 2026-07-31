#include <iostream>
#include <chrono>

bool is_prime(int n) {
    if(n <= 1) return false;
    for(int i=2; i*i<=n; i++) {
        if(n % i == 0) return false;
    }
    return true;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    int count = 0;
    for(int i=0; i<5000; i++) {
        if(is_prime(i)) count++;
    }
    std::cout << "Primes up to 5000: " << count << "\n";
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << elapsed.count() << "\n";
    return 0;
}
