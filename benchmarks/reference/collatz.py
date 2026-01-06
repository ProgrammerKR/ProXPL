
import time

def collatz(n):
    steps = 0
    while n > 1:
        if n % 2 == 0:
            n = n // 2
        else:
            n = 3 * n + 1
        steps += 1
    return steps

if __name__ == "__main__":
    start = time.time()
    max_steps = 0
    for i in range(1, 20001):
        steps = collatz(i)
        if steps > max_steps: max_steps = steps
    print(f"Max Collatz steps: {max_steps}")
    elapsed = time.time() - start
    print(f"Time: {elapsed:.6f}")
