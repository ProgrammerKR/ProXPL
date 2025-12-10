import sys
import os
from core.compiler.compiler_pipeline import CompilerPipeline

def run_file(filename):
    if not os.path.exists(filename):
        print(f"Error: File '{filename}' not found.")
        sys.exit(1)

    # print(f"Executing {filename}...")
    pipeline = CompilerPipeline()
    try:
        with open(filename, 'r') as f:
            code = f.read()
        pipeline.run(code)
    except Exception as e:
        print(f"Runtime Error: {e}")
        sys.exit(1)
