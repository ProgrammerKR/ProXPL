package main
import ("fmt"; "time")
func isPrime(n int) bool {
    if n <= 1 { return false }
    for i := 2; i * i <= n; i++ {
        if n % i == 0 { return false }
    }
    return true
}
func main() {
    start := time.Now()
    count := 0
    for i := 0; i < 5000; i++ {
        if isPrime(i) { count++ }
    }
    fmt.Println("Primes up to 5000:", count)
    fmt.Printf("Time: %.6f\n", time.Since(start).Seconds())
}
