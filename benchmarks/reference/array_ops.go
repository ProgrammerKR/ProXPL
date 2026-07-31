package main
import ("fmt"; "time")
func main() {
    start := time.Now()
    var arr []int
    for i := 0; i < 50000; i++ { arr = append(arr, i) }
    totalSum := 0
    for i := 0; i < len(arr); i++ { totalSum += arr[i] }
    for i := 0; i < 25000; i++ { arr = arr[:len(arr)-1] }
    fmt.Println("Array Sum:", totalSum)
    fmt.Println("Array Length:", len(arr))
    fmt.Printf("Time: %.6f\n", time.Since(start).Seconds())
}
