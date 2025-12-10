import sys
from core.compiler.compiler_pipeline import CompilerPipeline

def run(args):
    # This might be redundant if main.py handles it, but good for modularity
    print(f"Executing {args.file}...")
    pipeline = CompilerPipeline()
    try:
        with open(args.file, 'r') as f:
            code = f.read()
        pipeline.run(code)
    except Exception as e:
        print(f"Runtime Error: {e}")
        sys.exit(1)
