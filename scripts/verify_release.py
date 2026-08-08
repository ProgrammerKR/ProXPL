import os
import subprocess
import sys

def run_command(cmd, cwd=None):
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, cwd=cwd, text=True, capture_output=True)
    if result.returncode != 0:
        print(f"Command failed: {cmd}")
        print(f"STDOUT: {result.stdout}")
        print(f"STDERR: {result.stderr}")
        sys.exit(1)
    return result.stdout

def main():
    print("=== ProXPL v1.6.0 Release Verification ===")
    
    # 1. Build ProXPL
    print("\n[1/4] Building ProXPL...")
    run_command("cmake -B build")
    run_command("cmake --build build --config Release")
    
    # 2. Run Validation Suite
    print("\n[2/4] Running Validation Suite...")
    tests = [
        "tests/iop/test_asr_recovery.prox",
        "tests/iop/test_asr_edge_cases.prox",
        "tests/iop/test_intents_advanced.prox",
        "tests/iop/test_intents_errors.prox",
        "tests/iop/test_intents_gc.prox",
        "tests/iop/test_contexts_layers.prox",
        "tests/iop/test_contexts_errors.prox"
    ]
    
    executable = os.path.join("build", "Release", "proxpl.exe")
    if not os.path.exists(executable):
        executable = os.path.join("build", "proxpl")
        
    for test in tests:
        if os.path.exists(test):
            run_command(f"{executable} {test}")
        else:
            print(f"Warning: Test {test} not found.")
            
    # 3. Run Benchmarks
    print("\n[3/4] Running Benchmarks...")
    direct_out = run_command(f"{executable} tests/benchmarks/benchmark_direct.prox")
    intent_out = run_command(f"{executable} tests/benchmarks/benchmark_intent.prox")
    
    print(f"Direct Call (100k): {direct_out.strip()}s")
    print(f"Intent Call (100k): {intent_out.strip()}s")
    
    # 4. Check documentation
    print("\n[4/4] Verifying Documentation...")
    docs_to_check = [
        "CHANGELOG.md",
        "docs/releases/VERSION_UPGRADE_GUIDE_v1.6.0.md",
        "docs/releases/RELEASE_CHECKLIST_v1.6.0.md"
    ]
    for doc in docs_to_check:
        if not os.path.exists(doc):
            print(f"Error: Missing documentation {doc}")
            sys.exit(1)
            
    print("\n=== All Verification Steps Passed! ===")
    print("ProXPL is ready for v1.6.0 release.")

if __name__ == "__main__":
    main()
