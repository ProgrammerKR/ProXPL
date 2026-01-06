import time

def main():
    start = time.time()
    arr = []
    for i in range(50000):
        arr.append(i)
    
    total_sum = 0
    for i in range(len(arr)):
        total_sum += arr[i]
    
    # Pop half
    for i in range(25000):
        arr.pop()
    
    print(f"Array Sum: {total_sum}")
    print(f"Array Length: {len(arr)}")
    elapsed = time.time() - start
    print(f"Time: {elapsed:.6f}")

if __name__ == "__main__":
    main()
