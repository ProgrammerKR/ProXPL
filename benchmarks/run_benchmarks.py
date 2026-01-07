
import argparse
import subprocess
import time
import os
import sys
import glob
import shutil

# Configuration
REFERENCE_DIR = "reference"
MICRO_DIR = "micro"
MACRO_DIR = "macro"

# Output formatting
HEADER_FMT = "{:<20} | {:<10} | {:<10} | {:<10} | {:<10} | {:<10} | {:<10}"
ROW_FMT =    "{:<20} | {:<10} | {:<10} | {:<10} | {:<10} | {:<10} | {:<10}"
DIVIDER = "-" * 100

def run_cmd(cmd, timeout=300, cwd=None):
    start = time.time()
    try:
        proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout, cwd=cwd)
        duration = time.time() - start
        if proc.returncode != 0:
            return None, proc.stderr.decode('utf-8', errors='replace')
        return duration, proc.stdout.decode('utf-8', errors='replace')
    except subprocess.TimeoutExpired:
        return 'Timeout', "Timed out"
    except FileNotFoundError:
        return 'Missing', "Command not found"
    except Exception as e:
        return None, str(e)

def parse_time_from_output(output):
    # Try to find "Time: X.XXXXXX" in output
    for line in output.splitlines():
        if "Time:" in line:
            try:
                parts = line.split("Time:")
                return float(parts[1].strip().split()[0]) # distinct handle "0.123 s" if unit exists
            except:
                pass
    return None

def compile_cpp(src_file, bin_file):
    # g++ -O3 src -o bin
    return run_cmd(["g++", "-O3", src_file, "-o", bin_file])

def compile_go(src_file, bin_file):
    # go build -o bin src
    return run_cmd(["go", "build", "-o", bin_file, src_file])

def compile_java(src_file):
    # javac src
    return run_cmd(["javac", src_file])

def main():
    parser = argparse.ArgumentParser(description="Run ProXPL benchmarks.")
    parser.add_argument("--executable", required=True, help="Path to proXPL executable")
    args = parser.parse_args()

    if not os.path.exists(args.executable):
        print(f"Error: Executable not found at {args.executable}")
        sys.exit(1)

    executable = os.path.abspath(args.executable)
    base_dir = os.path.dirname(os.path.abspath(__file__))
    ref_dir = os.path.join(base_dir, REFERENCE_DIR)
    
    # Collect benchmarks
    benchmarks = []
    for d in [MICRO_DIR, MACRO_DIR]:
        path = os.path.join(base_dir, d)
        if os.path.exists(path):
            files = glob.glob(os.path.join(path, "*.prox"))
            benchmarks.extend([(f, d) for f in files])

    if not benchmarks:
        print("No benchmarks found.")
        sys.exit(0)

    print(HEADER_FMT.format("Benchmark", "ProXPL", "Python", "Node", "C++", "Java", "Go"))
    print(DIVIDER)

    failure = False
    
    # Check tool availability once
    has_python = shutil.which("python") is not None
    has_node = shutil.which("node") is not None
    has_gpp = shutil.which("g++") is not None
    has_javac = shutil.which("javac") is not None
    has_java = shutil.which("java") is not None
    has_go = shutil.which("go") is not None

    for bench_file, category in benchmarks:
        name = os.path.basename(bench_file).replace('.prox', '')
        
        # 1. Run ProXPL
        dur_prox, out_prox = run_cmd([executable, bench_file])
        parsed_prox = parse_time_from_output(out_prox) if isinstance(dur_prox, float) else dur_prox
        
        if parsed_prox is None:
             print(f"\n[ERROR] ProXPL failed for {name}:")
             print(out_prox)
             print("-" * 20)

        # 2. Run Python
        py_file = os.path.join(ref_dir, name + ".py")
        parsed_py = "N/A"
        if has_python and os.path.exists(py_file):
            dur, out = run_cmd(["python", py_file])
            parsed_py = parse_time_from_output(out) if isinstance(dur, float) else dur

        # 3. Run Node
        js_file = os.path.join(ref_dir, name + ".js")
        parsed_node = "N/A"
        if has_node and os.path.exists(js_file):
            dur, out = run_cmd(["node", js_file])
            parsed_node = parse_time_from_output(out) if isinstance(dur, float) else dur

        # 4. Run C++
        cpp_file = os.path.join(ref_dir, name + ".cpp")
        cpp_bin = os.path.join(ref_dir, name + "_cpp.exe" if os.name == 'nt' else name + "_cpp")
        parsed_cpp = "N/A"
        if has_gpp and os.path.exists(cpp_file):
            # Compile
            comp_dur, comp_err = compile_cpp(cpp_file, cpp_bin)
            if not comp_err:
                dur, out = run_cmd([cpp_bin])
                parsed_cpp = parse_time_from_output(out) if isinstance(dur, float) else dur
            else:
                parsed_cpp = "CompErr"

        # 5. Run Java
        java_src = os.path.join(ref_dir, name + ".java")
        parsed_java = "N/A"
        if has_javac and has_java and os.path.exists(java_src):
            # Compile
            comp_dur, comp_err = compile_java(java_src)
            if not comp_err:
                # Run (Class name matches file name assumed)
                dur, out = run_cmd(["java", "-cp", ref_dir, name])
                parsed_java = parse_time_from_output(out) if isinstance(dur, float) else dur
            else:
                parsed_java = "CompErr"

        # 6. Run Go
        go_src = os.path.join(ref_dir, name + ".go")
        go_bin = os.path.join(ref_dir, name + "_go.exe" if os.name == 'nt' else name + "_go")
        parsed_go = "N/A"
        if has_go and os.path.exists(go_src):
            # Compile
            comp_dur, comp_err = compile_go(go_src, go_bin)
            if not comp_err:
                dur, out = run_cmd([go_bin])
                parsed_go = parse_time_from_output(out) if isinstance(dur, float) else dur
            else:
                parsed_go = "CompErr"
        
        # Helper to format
        def fmt(val):
            if isinstance(val, float): return f"{val:.4f}"
            if val is None: return "Fail"
            return str(val)

        print(ROW_FMT.format(
            name, 
            fmt(parsed_prox), 
            fmt(parsed_py), 
            fmt(parsed_node), 
            fmt(parsed_cpp), 
            fmt(parsed_java), 
            fmt(parsed_go)
        ))

        if fmt(parsed_prox) in ["Fail", "Timeout"]:
            failure = True

    if failure:
        sys.exit(1)

if __name__ == "__main__":
    main()
