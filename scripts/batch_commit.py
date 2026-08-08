import subprocess
import time
import os

def run_cmd(cmd):
    result = subprocess.run(cmd, shell=True, text=True, capture_output=True)
    return result.stdout.strip(), result.stderr.strip(), result.returncode

def main():
    print("Gathering files to commit...")
    stdout, _, _ = run_cmd("git ls-files --modified --deleted --others --exclude-standard")
    
    files = stdout.split("\n")
    committed_count = 0
    
    for filepath in files:
        filepath = filepath.strip()
        if not filepath:
            continue
            
        print(f"Committing: {filepath}")
        run_cmd(f'git add "{filepath}"')
        
        diff, _, _ = run_cmd("git diff --cached --name-only")
        if not diff:
            continue
            
        msg = f"Add/Update {filepath.split('/')[-1]}"
        out, err, code = run_cmd(f'git commit -m "{msg}"')
        
        if code == 0:
            committed_count += 1
            print(f"Successfully committed {filepath}.")
        else:
            print(f"Failed to commit {filepath}: {err}")
            
        # Avoid Windows file lock contention
        time.sleep(0.1)
            
    print(f"Committed {committed_count} new files.")
    
    print("Pulling remote changes...")
    run_cmd("git pull origin main --rebase")
    
    print("Pushing...")
    for attempt in range(3):
        out, err, code = run_cmd("git push origin main")
        if code == 0:
            print("Push successful!")
            return
        else:
            print(f"Push failed (attempt {attempt + 1}). Retrying...")
            time.sleep(2)
            run_cmd("git pull origin main --rebase")
            
    print("Push permanently failed.")

if __name__ == "__main__":
    main()
