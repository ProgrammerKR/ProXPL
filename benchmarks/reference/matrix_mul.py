
import time

if __name__ == "__main__":
    start = time.time()
    size = 50
    a = []
    b = []
    
    for i in range(size):
        row_a = []
        row_b = []
        for j in range(size):
            row_a.append(i + j)
            row_b.append(i - j)
        a.append(row_a)
        b.append(row_b)
    
    c = []
    for i in range(size):
        row_c = []
        for j in range(size):
            sum_val = 0
            for k in range(size):
                sum_val += a[i][k] * b[k][j]
            row_c.append(sum_val)
        c.append(row_c)
        
    print(f"Matrix[0][0]: {c[0][0]}")
    elapsed = time.time() - start
    print(f"Time: {elapsed:.6f}")
