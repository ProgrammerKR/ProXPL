import os
import glob

def clean_builds():
    """Removes all .prob (binary) and temp files."""
    extensions = ['*.prob', '*.log', '*.tmp']
    count = 0
    for ext in extensions:
        files = glob.glob(f"**/{ext}", recursive=True)
        for f in files:
            try:
                os.remove(f)
                print(f"Removed {f}")
                count += 1
            except OSError as e:
                print(f"Error deleting {f}: {e}")
    print(f"Clean complete. Removed {count} files.")

if __name__ == '__main__':
    clean_builds()
