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
    
    # Use a temporary file for stdout/stderr to avoid MemoryError on huge output
    import tempfile
    
    with tempfile.TemporaryFile() as out_tmp:
        try:
            # We don't use check=True so we can capture output even on failure
            proc = subprocess.run(cmd, stdout=out_tmp, stderr=subprocess.STDOUT, timeout=60, text=False) # raw bytes
            
            end = time.time()
            duration = end - start
            
            # Read output back (limit size)
            out_tmp.seek(0)
            # Read first 4KB and last 4KB if large? Or just read all and catch error?
            # Let's read first 100KB which should be enough for any reasonable benchmark
            # If it's huge, we truncate.
            output_bytes = out_tmp.read(100 * 1024) 
            if out_tmp.read(1): # check if there is more
                 output_bytes += b"\n... [Output Truncated] ...\n"
                 
            output = output_bytes.decode('utf-8', errors='replace')
            
            if proc.returncode != 0:
                print(f"Error running {benchmark_file}: Return Code {proc.returncode}")
                # Print output snippet
                print(f"Output:\n{output[-1000:]}") # Last 1000 chars
                return None, None
                
            return duration, output

        except subprocess.TimeoutExpired:
            print(f"Benchmark {benchmark_file} timed out.")
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

    print(f"{'Benchmark':<30} | {'Time (s)':<10} | {'Status':<10}", flush=True)
    print("-" * 55, flush=True)

    failure = False
    for bench in benchmarks:
        name = os.path.basename(bench)
        duration, output = run_benchmark(args.executable, bench)
        
        if duration is not None:
             print(f"{name:<30} | {duration:<10.4f} | {'PASS':<10}", flush=True)
        else:
             print(f"{name:<30} | {'N/A':<10} | {'FAIL':<10}", flush=True)
             failure = True

    if failure:
        sys.exit(1)

if __name__ == "__main__":
    main()
