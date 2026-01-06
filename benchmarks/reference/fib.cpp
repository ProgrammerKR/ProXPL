
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

long long fib(int n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    auto start = chrono::high_resolution_clock::now();
    
    cout << "Fibonacci(30): " << fib(30) << endl;
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    
    cout << "Time: " << elapsed.count() << endl;
    return 0;
}
