import time

def merge(left, right):
    result = []
    i = 0
    j = 0
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1
    while i < len(left):
        result.append(left[i])
        i += 1
    while j < len(right):
        result.append(right[j])
        j += 1
    return result

def merge_sort(arr):
    if len(arr) <= 1:
        return arr
    mid = len(arr) // 2
    left = arr[:mid]
    right = arr[mid:]
    return merge(merge_sort(left), merge_sort(right))

def main():
    start = time.time()
    arr = [5000 - i for i in range(5000)]
    sorted_arr = merge_sort(arr)
    elapsed = time.time() - start
    print(f"First: {sorted_arr[0]}")
    print(f"Last: {sorted_arr[-1]}")
    print(f"Time: {elapsed:.6f}")

if __name__ == "__main__":
    main()
