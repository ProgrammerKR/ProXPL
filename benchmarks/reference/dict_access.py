import time

def main():
    start = time.time()
    d = {}
    for i in range(10000):
        d["key" + str(i)] = i
    
    total_sum = 0
    for i in range(10000):
        total_sum += d["key" + str(i)]
    
    print(f"Dict Sum: {total_sum}")
    elapsed = time.time() - start
    print(f"Time: {elapsed:.6f}")

if __name__ == "__main__":
    main()
