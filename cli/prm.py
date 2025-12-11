import argparse
import sys
import os
import json
import shutil
import urllib.request
from typing import Dict, Any

# Configuration
PACKAGES_DIR = "packages"
REGISTRY_FILE = os.path.join(PACKAGES_DIR, "registry.json")

# Mock Registry for "Network" Simulation (as per requirements for now)
# In future, this would request a real URL.
MOCK_PUBLIC_REGISTRY = {
    "utils/io": {
        "version": "1.0.0",
        "description": "Standard Input/Output utilities",
        "content": """
func print_line(msg) {
    print(msg);
}

func read_input(prompt) {
    // System.input is not yet stdlib, assuming generic native call
    print(prompt);
    // Mock return
    return "user input";
}
"""
    },
    "math/advanced": {
        "version": "0.5.0",
        "description": "Advanced mathematical functions",
        "content": """
func square(x) { 
    return x * x; 
}
func cube(x) {
    return x * x * x;
}
"""
    },
     "colors": {
        "version": "1.2.0",
        "description": "Terminal colors and formatting",
        "content": """
const RED = "\u001b[31m";
const GREEN = "\u001b[32m";
const BLUE = "\u001b[34m";
const RESET = "\u001b[0m";

func red(text) {
    return RED + text + RESET;
}
"""
    }
}

def ensure_packages_dir():
    if not os.path.exists(PACKAGES_DIR):
        try:
            os.makedirs(PACKAGES_DIR)
            print(f"Initialized local package directory: {PACKAGES_DIR}/")
        except OSError as e:
            print(f"Error creating package directory: {e}")
            sys.exit(1)

def load_registry() -> Dict[str, Any]:
    ensure_packages_dir()
    if not os.path.exists(REGISTRY_FILE):
        return {}
    try:
        with open(REGISTRY_FILE, 'r') as f:
            return json.load(f)
    except json.JSONDecodeError:
        print(f"Warning: Registry file '{REGISTRY_FILE}' is corrupted. Resetting.")
        return {}
    except Exception as e:
        print(f"Error loading registry: {e}")
        return {}

def save_registry(registry: Dict[str, Any]):
    ensure_packages_dir()
    try:
        with open(REGISTRY_FILE, 'w') as f:
            json.dump(registry, f, indent=4)
    except Exception as e:
        print(f"Error saving registry: {e}")

def install_pkg(pkg_name: str, force: bool = False):
    print(f"Resolving '{pkg_name}'...")
    registry = load_registry()
    
    if pkg_name in registry and not force:
        print(f"Package '{pkg_name}' (v{registry[pkg_name]['version']}) is already installed.")
        print("Use 'prm update' to upgrade or install with --force.")
        return
        
    # Simulate network fetch from "Public Registry"
    if pkg_name in MOCK_PUBLIC_REGISTRY:
        pkg_data = MOCK_PUBLIC_REGISTRY[pkg_name]
        version = pkg_data['version']
        content = pkg_data['content']
        
        # Save file
        # Handle path structure: "utils/io" -> packages/prm_utils_io.prox or packages/utils/io.prox?
        # Requirement: "./packages/<module>.prox"
        # If module is "utils/io", file should probably be "packages/utils/io.prox".
        
        dest_path = os.path.join(PACKAGES_DIR, f"{pkg_name}.prox")
        dest_dir = os.path.dirname(dest_path)
        
        if dest_dir and not os.path.exists(dest_dir):
            os.makedirs(dest_dir, exist_ok=True)
            
        try:
            with open(dest_path, 'w') as f:
                f.write(content.strip())
        except Exception as e:
            print(f"Error writing package file: {e}")
            return
            
        # Update registry
        registry[pkg_name] = {
            "version": version,
            "installed_at": "local", # timestamp could go here
            "description": pkg_data.get("description", "")
        }
        save_registry(registry)
        print(f"Successfully installed '{pkg_name}' v{version}.")
    else:
        print(f"Error: Package '{pkg_name}' not found in the registry.")

def list_pkgs():
    registry = load_registry()
    if not registry:
        print("No packages installed in this project.")
        print("Use 'prm install <package>' to add one.")
    else:
        print(f"Installed Packages ({len(registry)}):")
        print(f"{'Name':<20} {'Version':<10} {'Description'}")
        print("-" * 60)
        for pkg, meta in registry.items():
            desc = meta.get('description', 'No description')
            print(f"{pkg:<20} {meta['version']:<10} {desc}")

def remove_pkg(pkg_name: str):
    registry = load_registry()
    if pkg_name not in registry:
        print(f"Package '{pkg_name}' is not installed.")
        return
    
    # Remove file
    filename = os.path.join(PACKAGES_DIR, f"{pkg_name}.prox")
    if os.path.exists(filename):
        try:
            os.remove(filename)
            print(f"Removed file {filename}")
        except Exception as e:
            print(f"Error removing package file: {e}")
            
        # Cleanup empty directories
        directory = os.path.dirname(filename)
        # Verify directory is inside PACKAGES_DIR to avoid deleting random sys dirs
        abs_pkg_dir = os.path.abspath(PACKAGES_DIR)
        abs_dir = os.path.abspath(directory)
        
        if abs_dir.startswith(abs_pkg_dir) and abs_dir != abs_pkg_dir:
            if not os.listdir(directory):
                try:
                    os.rmdir(directory)
                    print(f"Cleaned up empty directory {directory}")
                except: pass
                
    del registry[pkg_name]
    save_registry(registry)
    print(f"Package '{pkg_name}' uninstalled.")

def search_pkg(keyword: str):
    print(f"Searching for '{keyword}' in public registry...")
    results = []
    
    # Search in Mock Registry
    for pkg, data in MOCK_PUBLIC_REGISTRY.items():
        if keyword.lower() in pkg.lower() or keyword.lower() in data.get('description', '').lower():
            results.append((pkg, data['version'], data.get('description', '')))
            
    if results:
        print(f"Found {len(results)} matches:")
        for name, ver, desc in results:
            print(f" - {name} (v{ver}): {desc}")
    else:
        print("No packages found matching your criteria.")

def update_pkg(pkg_name: str = None):
    registry = load_registry()
    if not registry:
        print("No packages installed to update.")
        return

    to_update = [pkg_name] if pkg_name else list(registry.keys())
    
    for pkg in to_update:
        if pkg not in registry and pkg_name:
             print(f"Package '{pkg}' is not installed.")
             continue
             
        print(f"Checking updates for '{pkg}'...")
        # In a real scenario, compare registry[pkg]['version'] with remote version
        # Here we just reinstall/overwrite
        install_pkg(pkg, force=True)

def main():
    parser = argparse.ArgumentParser(description="PRM - ProX Repository Manager")
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # install
    p_install = subparsers.add_parser('install', help='Install a package')
    p_install.add_argument('package', help='Package name (e.g., utils/io)')
    p_install.add_argument('--force', action='store_true', help='Force reinstall')
    
    # list
    p_list = subparsers.add_parser('list', help='List installed packages')
    
    # remove
    p_remove = subparsers.add_parser('remove', help='Remove a package')
    p_remove.add_argument('package', help='Package name')
    p_remove.add_argument('--yes', '-y', action='store_true', help='Skip confirmation') # Not impl yet but good practice
    
    # search
    p_search = subparsers.add_parser('search', help='Search for packages')
    p_search.add_argument('keyword', help='Search keyword')

    # update
    p_update = subparsers.add_parser('update', help='Update packages')
    p_update.add_argument('package', nargs='?', help='Specific package to update (optional)')

    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)

    args = parser.parse_args()
    
    try:
        if args.command == 'install':
            install_pkg(args.package, getattr(args, 'force', False))
        elif args.command == 'list':
            list_pkgs()
        elif args.command == 'remove':
            remove_pkg(args.package)
        elif args.command == 'search':
            search_pkg(args.keyword)
        elif args.command == 'update':
            update_pkg(args.package)
        else:
            parser.print_help()
    except KeyboardInterrupt:
        print("\nOperation cancelled.")
        sys.exit(1)

if __name__ == '__main__':
    main()
