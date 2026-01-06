
package main

import (
	"fmt"
	"time"
)

func fib(n int) int {
	if n < 2 {
		return n
	}
	return fib(n-1) + fib(n-2)
}

func main() {
	start := time.Now()
	
	fmt.Printf("Fibonacci(30): %d\n", fib(30))
	
	elapsed := time.Since(start).Seconds()
	fmt.Printf("Time: %.6f\n", elapsed)
}
