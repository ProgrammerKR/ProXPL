package main
import ("fmt"; "time")
func main() {
    start := time.Now()
    size := 50
    a := make([][]int, size)
    b := make([][]int, size)
    for i := 0; i < size; i++ {
        a[i] = make([]int, size)
        b[i] = make([]int, size)
        for j := 0; j < size; j++ {
            a[i][j] = i + j
            b[i][j] = i - j
        }
    }
    c := make([][]int, size)
    for i := 0; i < size; i++ {
        c[i] = make([]int, size)
        for j := 0; j < size; j++ {
            sumVal := 0
            for k := 0; k < size; k++ {
                sumVal += a[i][k] * b[k][j]
            }
            c[i][j] = sumVal
        }
    }
    fmt.Println("Matrix[0][0]:", c[0][0])
    fmt.Printf("Time: %.6f\n", time.Since(start).Seconds())
}
