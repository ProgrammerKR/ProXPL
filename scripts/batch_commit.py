import subprocess
import time

def run_cmd(cmd):
    result = subprocess.run(cmd, shell=True, text=True, capture_output=True)
    return result.stdout.strip(), result.stderr.strip(), result.returncode

def main():
    print("Gathering files to commit...")
    # Get all modified, deleted, and untracked files
    stdout, _, _ = run_cmd("git ls-files --modified --deleted --others --exclude-standard")
    
    files = stdout.split("\\n")
    committed_count = 0
    
    for filepath in files:
        filepath = filepath.strip()
        if not filepath:
            continue
            
        print(f"Committing: {filepath}")
        
        # Add the file (handles both modified and untracked)
        # To handle deleted files properly, use git add -A <file> or just git add
        run_cmd(f'git add -A "{filepath}"')
        
        # Check if there is anything staged for this file
        diff, _, _ = run_cmd("git diff --cached --name-only")
        if not diff:
            continue
            
        msg = f"Update {filepath.split('/')[-1]}"
        commit_out, commit_err, code = run_cmd(f'git commit -m "{msg}"')
        
        if code == 0:
            committed_count += 1
            
    print(f"Successfully committed {committed_count} files individually.")
    
    print("Pulling remote changes...")
    run_cmd("git pull origin main --rebase")
    
    print("Pushing...")
    # Try multiple times if push fails due to lock
    for attempt in range(5):
        out, err, code = run_cmd("git push origin main")
        if code == 0:
            print("Push successful!")
            return
        else:
            print(f"Push failed (attempt {attempt + 1}). Retrying in 2 seconds...")
            time.sleep(2)
            run_cmd("git pull origin main --rebase")
            
    print("Push permanently failed.")

if __name__ == "__main__":
    main()
