
import time

def is_prime(n):
    if n <= 1: return False
    i = 2
    while i * i <= n:
        if n % i == 0: return False
        i += 1
    return True

if __name__ == "__main__":
    start = time.time()
    count = 0
    for i in range(5000):
        if is_prime(i):
            count += 1
    print(f"Primes up to 5000: {count}")
    elapsed = time.time() - start
    print(f"Time: {elapsed:.6f}")
