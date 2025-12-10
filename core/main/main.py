import argparse
import sys
import os

# Add parent directory to path so imports work
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from core.compiler.compiler_pipeline import CompilerPipeline

def main():
    parser = argparse.ArgumentParser(description="ProXPL Compiler & Runtime")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")

    # run command
    run_parser = subparsers.add_parser("run", help="Run a ProXPL source file")
    run_parser.add_argument("file", help="Path to .prox file")

    # build command
    build_parser = subparsers.add_parser("build", help="Compile a ProXPL source file to bytecode")
    build_parser.add_argument("file", help="Path to .prox file")
    build_parser.add_argument("--out", "-o", help="Output file path")

    args = parser.parse_args()

    pipeline = CompilerPipeline()

    if args.command == "run":
        if not os.path.exists(args.file):
            print(f"Error: File '{args.file}' not found.")
            sys.exit(1)
        
        with open(args.file, "r") as f:
            source = f.read()
            
        print(f"--- Running {args.file} ---")
        pipeline.run(source)

    elif args.command == "build":
        if not os.path.exists(args.file):
            print(f"Error: File '{args.file}' not found.")
            sys.exit(1)
            
        with open(args.file, "r") as f:
            source = f.read()
            
        print(f"--- Building {args.file} ---")
        # pipeline.compile(source) -> serialize -> save
        print("Build implementation pending serialization support.")
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
