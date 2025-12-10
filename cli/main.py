import sys
import os
import argparse
from cli.commands.run import run_file
from cli.commands.build import build_file

def main():
    """
    Main entry point for the 'prox' command-line tool.
    usage: prox [filename.prox] | prox [command] [args]
    """
    
    # If standard args are provided with potential subcommands
    parser = argparse.ArgumentParser(description="ProXPL - The Professional Educational Programming Language")
    subparsers = parser.add_subparsers(dest='command', help='Available commands')

    # 'run' subcommand
    parser_run = subparsers.add_parser('run', help='Run a .prox file')
    parser_run.add_argument('file', help='Path to the .prox file')
    
    # 'build' subcommand
    parser_build = subparsers.add_parser('build', help='Build/Compile a .prox file')
    parser_build.add_argument('file', help='Path to the .prox file')

    # Check if the first arg looks like a file (ends with .prox) and no subcommand is explicitly invoked
    # This allows `prox myfile.prox` shortcut
    if len(sys.argv) > 1 and sys.argv[1].endswith('.prox') and not sys.argv[1] in ['run', 'build']:
        # Implicit run
        filename = sys.argv[1]
        sys.argv.pop(1) # Remove filename from args to not confuse argparse if we were to parse again, 
                        # but here we just call run directly
        run_file(filename)
        return

    args = parser.parse_args()

    if args.command == 'run':
        run_file(args.file)
    elif args.command == 'build':
        build_file(args.file)
    else:
        # If no arguments or unknown command, check if user provided a file without .prox extension but meant to run it?
        # For now, just print help
        if len(sys.argv) == 1:
            parser.print_help()
        else:
            # Handle case where user provided something that isn't a command and doesn't end in .prox
            # We could try to interpret it as a file, but let's be strict for now or show help
            print(f"Unknown command or file: {sys.argv[1]}")
            print("Usage: prox filename.prox")
            print("       prox run filename.prox")
            print("       prox build filename.prox")
            sys.exit(1)

if __name__ == '__main__':
    main()
