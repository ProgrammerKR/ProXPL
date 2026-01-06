import os
import re

ROOT_DIR = r"c:\Users\INTEX\Desktop\Kanishk Raj\Coding\ProXPL"

def resolve_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"Skipping {filepath}: {e}")
        return

    if "<<<<<<< HEAD" not in content:
        return

    print(f"Resolving {filepath}...")

    # Regex to capture the HEAD part and ignore the rest
    # Pattern: <<<<<<< HEAD\n(content)\n=======\n(other_content)\n>>>>>>> branch
    # We want to keep (content).
    # Note: re.DOTALL is needed for dots to match newlines.
    
    # This regex assumes standard git conflict markers.
    # It attempts to match the whole block.
    # We use non-greedy match *? for content.
    
    pattern = re.compile(r'<<<<<<< HEAD\n(.*?)\n=======\n.*?\n>>>>>>> .*?\n', re.DOTALL)
    
    # Sometimes markers don't have newlines immediately if EOF etc? Usually they do.
    # Let's try a robust line-by-line approach instead of regex for safety against nested weirdness or partial matching.
    
    lines = content.splitlines(keepends=True)
    new_lines = []
    in_head = False
    in_other = False
    
    for line in lines:
        if line.startswith("<<<<<<< HEAD"):
            in_head = True
            in_other = False
            continue
        elif line.startswith("======="):
            if in_head:
                in_head = False
                in_other = True
            else:
                # Suspicious: ======= without HEAD? 
                # Could be string literal? Git markers usually start at beginning of line.
                new_lines.append(line)
            continue
        elif line.startswith(">>>>>>>"):
            if in_other:
                in_other = False
            else:
                 # Suspicious
                 new_lines.append(line)
            continue
            
        if in_head:
            new_lines.append(line)
        elif in_other:
            continue # Skip lines in the incoming branch
        else:
            new_lines.append(line)
            
    with open(filepath, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)

def main():
    for root, dirs, files in os.walk(ROOT_DIR):
        if ".git" in info:
           continue
        for file in files:
            # Skip binary extensions if needed, but text checking handles 'ignore' errors
            resolve_file(os.path.join(root, file))

if __name__ == "__main__":
    # Fix 'info' undefined in loop
    # os.walk yields (root, dirs, files)
    # Filter dirs to exclude .git
    pass 

# Retrying with correct loop
def main_corrected():
    print("Starting resolution...")
    for root, dirs, files in os.walk(ROOT_DIR):
        if ".git" in dirs:
            dirs.remove(".git")
            
        for file in files:
            path = os.path.join(root, file)
            resolve_file(path)
    print("Done.")

if __name__ == "__main__":
    main_corrected()
