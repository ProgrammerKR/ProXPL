
import os
import sys
import shutil
import subprocess

# Setup paths
PROX_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLI_MAIN = os.path.join(PROX_ROOT, "cli", "main.py")
PRM_CLI = os.path.join(PROX_ROOT, "cli", "prm.py")
PACKAGES_DIR = os.path.join(PROX_ROOT, "packages")

def run_prox(file_path, cwd=None):
    env = os.environ.copy()
    env["PYTHONPATH"] = PROX_ROOT + os.pathsep + env.get("PYTHONPATH", "")
    cmd = [sys.executable, CLI_MAIN, "run", file_path]
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=cwd, env=env)
    if result.returncode != 0:
         print(f"PROX STDERR: {result.stderr}")
         print(f"PROX STDOUT: {result.stdout}")
    return result

def run_prm(args):
    cmd = [sys.executable, PRM_CLI] + args
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"PRM STDERR: {result.stderr}")
    return result

def test_prm():
    print("--- Testing PRM ---")
    # Clean
    if os.path.exists(PACKAGES_DIR):
        shutil.rmtree(PACKAGES_DIR)
    
    # Install
    res = run_prm(["install", "utils/io"])
    print(f"Install: {'OK' if res.returncode == 0 else 'FAIL'} -> {res.stdout.strip()}")
    assert os.path.exists(os.path.join(PACKAGES_DIR, "utils", "io.prox"))
    
    # List
    res = run_prm(["list"])
    print(f"List: {'OK' if res.returncode == 0 else 'FAIL'} -> {res.stdout.strip()}")
    assert "utils/io" in res.stdout
    
    # Search
    res = run_prm(["search", "math"])
    print(f"Search: {'OK' if res.returncode == 0 else 'FAIL'} -> {res.stdout.strip()}")
    assert "math/advanced" in res.stdout

def test_modules():
    print("\n--- Testing Modules ---")
    
    # Create valid module chain
    os.makedirs("test_env", exist_ok=True)
    
    with open("test_env/main.prox", "w") as f:
        f.write('use libA;\nprint("Main Done");')
        
    with open("test_env/libA.prox", "w") as f:
        f.write('use libB;\nprint("LibA Loaded");')
        
    with open("test_env/libB.prox", "w") as f:
        f.write('print("LibB Loaded");')
        
    res = run_prox("main.prox", cwd="test_env")
    print(f"Valid Import: {'OK' if res.returncode == 0 else 'FAIL'}")
    
    if res.returncode == 0:
        print(res.stdout)
    
    # Create circular module
    with open("test_env/circular1.prox", "w") as f:
        f.write('use circular2;')
    with open("test_env/circular2.prox", "w") as f:
        f.write('use circular1;')
        
    res = run_prox("circular1.prox", cwd="test_env")
    print(f"Circular Import: {'OK' if 'Circular dependency detected' in res.stdout or 'circular dependency detected' in str(res.stderr).lower() or 'circular dependency detected' in str(res.stdout).lower() else 'FAIL'}")
    if 'Circular' not in res.stdout and 'Circular' not in str(res.stderr):
        print("Expected circular error, got:")
        print(res.stdout)
        print(res.stderr)

    # Clean
    shutil.rmtree("test_env")
    if os.path.exists(PACKAGES_DIR):
        shutil.rmtree(PACKAGES_DIR)

if __name__ == "__main__":
    test_prm()
    test_modules()
