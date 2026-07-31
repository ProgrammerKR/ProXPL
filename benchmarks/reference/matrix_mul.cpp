#include <iostream>
#include <vector>
#include <chrono>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    int size = 50;
    std::vector<std::vector<int>> a(size, std::vector<int>(size));
    std::vector<std::vector<int>> b(size, std::vector<int>(size));
    
    for(int i=0; i<size; i++) {
        for(int j=0; j<size; j++) {
            a[i][j] = i + j;
            b[i][j] = i - j;
        }
    }
    
    std::vector<std::vector<int>> c(size, std::vector<int>(size));
    for(int i=0; i<size; i++) {
        for(int j=0; j<size; j++) {
            int sum_val = 0;
            for(int k=0; k<size; k++) {
                sum_val += a[i][k] * b[k][j];
            }
            c[i][j] = sum_val;
        }
    }
    
    std::cout << "Matrix[0][0]: " << c[0][0] << "\n";
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << elapsed.count() << "\n";
    return 0;
}
