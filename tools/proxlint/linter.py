import sys
import re

class Linter:
    """
    Static analysis tool to find potential issues.
    """
    
    def lint_file(self, file_path):
        warnings = 0
        with open(file_path, 'r') as f:
            lines = f.readlines()
            
        for i, line in enumerate(lines):
            line_num = i + 1
            
            # Rule 1: TODOs
            if "TODO" in line:
                print(f"{file_path}:{line_num}: [Warning] Found TODO")
                warnings += 1
                
            # Rule 2: Line length
            if len(line) > 100:
                print(f"{file_path}:{line_num}: [Warning] Line too long (>100 chars)")
                warnings += 1
                
            # Rule 3: CamelCase check for class
            match = re.search(r'class\s+([a-z]\w*)', line)
            if match:
                print(f"{file_path}:{line_num}: [Style] Class '{match.group(1)}' should be PascalCase")
                warnings += 1

        if warnings == 0:
            print("No lint issues found.")
        else:
            print(f"Found {warnings} lint issues.")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python linter.py <file>")
    else:
        Linter().lint_file(sys.argv[1])
