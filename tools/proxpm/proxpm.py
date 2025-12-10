import argparse
import sys
import json
import os

class ProxPM:
    """
    ProXPL Package Manager (proxpm)
    Handles calculating dependencies, installing packages, etc.
    """
    
    def __init__(self):
        self.repo_url = "https://pkg.proxpl.dev/api"
        self.manifest = "prox.json"

    def init(self, name):
        if os.path.exists(self.manifest):
            print("Package already initialized.")
            return

        data = {
            "name": name,
            "version": "0.1.0",
            "dependencies": {},
            "author": ""
        }
        with open(self.manifest, 'w') as f:
            json.dump(data, f, indent=4)
        print(f"Initialized {name} in {self.manifest}")

    def install(self, package):
        print(f"Installing {package}...")
        # Simulate network fetch
        print(f"Downloaded {package} v1.0.0")
        
        # Add to manifest
        try:
            with open(self.manifest, 'r') as f:
                data = json.load(f)
            data['dependencies'][package] = "^1.0.0"
            with open(self.manifest, 'w') as f:
                json.dump(data, f, indent=4)
        except Exception:
            print("Warning: No prox.json found, skipping manifest update.")

def main():
    parser = argparse.ArgumentParser(description="ProX Package Manager")
    parser.add_argument("command", choices=["init", "install"])
    parser.add_argument("package", nargs="?", help="Package name to install")
    
    args = parser.parse_args()
    pm = ProxPM()

    if args.command == "init":
        pm.init(args.package or "my-app")
    elif args.command == "install":
        if not args.package:
            print("Please specify a package.")
        else:
            pm.install(args.package)

if __name__ == "__main__":
    main()
