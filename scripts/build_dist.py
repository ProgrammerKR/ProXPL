import PyInstaller.__main__
import os
import shutil
import sys

def build():
    print("Building ProXPL executable...")
    
    # Determine separator and extension based on OS
    is_windows = sys.platform.startswith('win')
    sep = ';' if is_windows else ':'
    
    # Clean previous builds
    if os.path.exists('build'):
        shutil.rmtree('build')
    if os.path.exists('dist'):
        shutil.rmtree('dist')
        
    # PyInstaller arguments
    args = [
        'cli/main.py',              # Script to build
        '--name=prox',              # Name of the executable
        '--onefile',                # Create a single executable file
        '--clean',                  # Clean cache
        # Add paths to hidden imports if necessary
        # '--hidden-import=core',
    ]
    
    # Run PyInstaller
    PyInstaller.__main__.run(args)
    
    print("Build complete.")
    print(f"Executable located at: dist/prox{' .exe' if is_windows else ''}")

if __name__ == "__main__":
    # Ensure we are in the project root
    if not os.path.exists('setup.py'):
        print("Error: Please run this script from the project root directory.")
        sys.exit(1)
        
    build()
