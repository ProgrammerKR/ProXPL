package main
import ("fmt"; "time")
func merge(left, right []int) []int {
    var result []int
    i, j := 0, 0
    for i < len(left) && j < len(right) {
        if left[i] <= right[j] { result = append(result, left[i]); i++ } else { result = append(result, right[j]); j++ }
    }
    for i < len(left) { result = append(result, left[i]); i++ }
    for j < len(right) { result = append(result, right[j]); j++ }
    return result
}
func mergeSort(arr []int) []int {
    if len(arr) <= 1 { return arr }
    mid := len(arr) / 2
    left := mergeSort(arr[:mid])
    right := mergeSort(arr[mid:])
    return merge(left, right)
}
func main() {
    start := time.Now()
    var arr []int
    for i := 0; i < 5000; i++ { arr = append(arr, 5000 - i) }
    sorted := mergeSort(arr)
    fmt.Println("First:", sorted[0])
    fmt.Println("Last:", sorted[len(sorted)-1])
    fmt.Printf("Time: %.6f\n", time.Since(start).Seconds())
}
