import sys
from cli.build.builder import Builder

def run(args):
    print(f"Building {args.file}...")
    builder = Builder()
    success = builder.build(args.file, args.out)
    if success:
        print("Build successful.")
    else:
        print("Build failed.")
        sys.exit(1)
