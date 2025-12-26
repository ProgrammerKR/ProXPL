import argparse
import subprocess
import time
import os
import sys
import glob

def run_benchmark(executable, benchmark_file):
    # Ensure absolute path for executable to avoid issues
    executable = os.path.abspath(executable)
    benchmark_file = os.path.abspath(benchmark_file)
    
    cmd = [executable, benchmark_file]
    start = time.time()
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        end = time.time()
        return end - start, result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running {benchmark_file}: {e}")
        print(f"Stdout: {e.stdout}")
        print(f"Stderr: {e.stderr}")
        return None, None
    except Exception as e:
        print(f"Execution failed: {e}")
        return None, None

def main():
    parser = argparse.ArgumentParser(description="Run ProXPL benchmarks.")
    parser.add_argument("--executable", required=True, help="Path to proXPL executable")
    args = parser.parse_args()

    if not os.path.exists(args.executable):
        print(f"Error: Executable not found at {args.executable}")
        sys.exit(1)

    # Find all .prox files in benchmarks directory
    bench_dir = os.path.dirname(os.path.abspath(__file__))
    benchmarks = glob.glob(os.path.join(bench_dir, "*.prox"))
    
    if not benchmarks:
         print("No .prox benchmark files found.")
         # Optional: don't fail if no benchmarks? 
         # But user expects benchmarks.
         return

    print(f"{'Benchmark':<30} | {'Time (s)':<10} | {'Status':<10}")
    print("-" * 55)

    failure = False
    for bench in benchmarks:
        name = os.path.basename(bench)
        duration, output = run_benchmark(args.executable, bench)
        
        if duration is not None:
             print(f"{name:<30} | {duration:<10.4f} | {'PASS':<10}")
        else:
             print(f"{name:<30} | {'N/A':<10} | {'FAIL':<10}")
             failure = True

    if failure:
        sys.exit(1)

if __name__ == "__main__":
    main()
