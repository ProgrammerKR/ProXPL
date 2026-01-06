
import time

def fib(n):
    if n < 2: return n
    return fib(n - 1) + fib(n - 2)

if __name__ == "__main__":
    start = time.time()
    print(f"Fibonacci(30): {fib(30)}")
    elapsed = time.time() - start
    print(f"Time: {elapsed:.6f}")
