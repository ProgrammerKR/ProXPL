#!/bin/bash

# Get the absolute path to the ProXPL directory
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CLI_Entry="$PROJECT_DIR/cli/main.py"

echo "Creating 'prox' executable wrapper..."

# Create the wrapper script
cat > prox <<EOF
#!/usr/bin/env python3
import sys
import os

# Add the project root to sys.path
sys.path.insert(0, "$PROJECT_DIR")

from cli.main import main

if __name__ == "__main__":
    main()
EOF

# Make it executable
chmod +x prox

echo "Successfully created 'prox' launcher."
echo "You can move it to /usr/local/bin or adds to your PATH."
echo "Usage: ./prox examples/hello.prox"
