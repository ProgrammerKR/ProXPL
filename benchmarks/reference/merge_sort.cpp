#include <iostream>
#include <vector>
#include <chrono>

std::vector<int> merge(const std::vector<int>& left, const std::vector<int>& right) {
    std::vector<int> result;
    size_t i = 0, j = 0;
    while(i < left.size() && j < right.size()) {
        if(left[i] <= right[j]) result.push_back(left[i++]);
        else result.push_back(right[j++]);
    }
    while(i < left.size()) result.push_back(left[i++]);
    while(j < right.size()) result.push_back(right[j++]);
    return result;
}

std::vector<int> merge_sort(const std::vector<int>& arr) {
    if(arr.size() <= 1) return arr;
    size_t mid = arr.size() / 2;
    std::vector<int> left(arr.begin(), arr.begin() + mid);
    std::vector<int> right(arr.begin() + mid, arr.end());
    return merge(merge_sort(left), merge_sort(right));
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> arr;
    for(int i=0; i<5000; i++) arr.push_back(5000 - i);
    std::vector<int> sorted = merge_sort(arr);
    
    std::cout << "First: " << sorted[0] << "\n";
    std::cout << "Last: " << sorted.back() << "\n";
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << elapsed.count() << "\n";
    return 0;
}
