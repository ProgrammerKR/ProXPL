import sys

class Formatter:
    """
    Opinionated code formatter for ProXPL.
    Currently a simple pass-through with indentation fixer simulation.
    """
    
    def format_file(self, file_path):
        with open(file_path, 'r') as f:
            lines = f.readlines()
            
        formatted = []
        indent = 0
        for line in lines:
            stripped = line.strip()
            if not stripped:
                formatted.append("")
                continue
            
            # Decrease indent before
            if stripped.startswith("}"):
                indent = max(0, indent - 1)
            
            # Apply indent
            formatted.append("    " * indent + stripped)
            
            # Increase indent after
            if stripped.endswith("{"):
                indent += 1
                
        with open(file_path, 'w') as f:
            f.write("\n".join(formatted))
        print(f"Formatted {file_path}")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python formatter.py <file>")
    else:
        Formatter().format_file(sys.argv[1])
