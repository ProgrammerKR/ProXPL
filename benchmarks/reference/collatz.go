package main
import ("fmt"; "time")
func collatz(n int) int {
    steps := 0
    for n > 1 {
        if n % 2 == 0 { n /= 2 } else { n = 3 * n + 1 }
        steps++
    }
    return steps
}
func main() {
    start := time.Now()
    maxSteps := 0
    for i := 1; i <= 20000; i++ {
        steps := collatz(i)
        if steps > maxSteps { maxSteps = steps }
    }
    fmt.Println("Max Collatz steps:", maxSteps)
    fmt.Printf("Time: %.6f\n", time.Since(start).Seconds())
}
