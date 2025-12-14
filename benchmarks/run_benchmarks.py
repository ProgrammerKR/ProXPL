import subprocess
import time
import os
import sys

def run_command(cmd):
    start = time.time()
    try:
        subprocess.run(cmd, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except subprocess.CalledProcessError as e:
        print(f"Error running {cmd}: {e}")
        return None
    end = time.time()
    return end - start

def main():
    benchmarks = [
        "benchmarks/fibonacci.prox",
        "benchmarks/loop_math.prox",
        "benchmarks/string_concat.prox"
    ]

    # Assume we have compiled proxpl in bin/proxpl (or use python version for now if bin not ready)
    # For now, we compare against CPython as specific in prompt (target numbers)
    
    print(f"{'Benchmark':<25} | {'ProXPL (s)':<10} | {'CPython (s)':<10} | {'Speedup':<10}")
    print("-" * 65)

    for bench in benchmarks:
        # Construct equivalent python command (assuming corresponding .py files exist or we generate them)
        # For simplicity, we just run the prox file with current proxpl interpreter if it works, 
        # but since we are replacing it, we might not have a working one yet.
        # This script is a template.
        
        prox_cmd = f"bin/proxpl run {bench}" 
        # py_cmd = f"python3 {bench.replace('.prox', '.py')}"
        
        # Placeholder Results
        print(f"{bench:<25} | {'N/A':<10} | {'1.20':<10} | {'Pending'}")

if __name__ == "__main__":
    main()
