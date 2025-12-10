import argparse
import sys
import os
import json
import shutil
import urllib.request

# Configuration
PACKAGES_DIR = "packages"
REGISTRY_FILE = os.path.join(PACKAGES_DIR, "registry.json")

# Mock Registry for "Network"
MOCK_PUBLIC_REGISTRY = {
    "utils/io": {
        "version": "1.0.0",
        "content": "func print_hello() { print(\"Hello from utils/io\"); }"
    },
    "math/advanced": {
        "version": "0.5.0",
        "content": "func square(x) { return x * x; }"
    },
     "colors": {
        "version": "1.2.0",
        "content": "const RED = \"#FF0000\"; const BLUE = \"#0000FF\";"
    }
}

def ensure_packages_dir():
    if not os.path.exists(PACKAGES_DIR):
        os.makedirs(PACKAGES_DIR)
        print(f"Created {PACKAGES_DIR}/")

def load_registry():
    ensure_packages_dir()
    if not os.path.exists(REGISTRY_FILE):
        save_registry({})
        return {}
    try:
        with open(REGISTRY_FILE, 'r') as f:
            return json.load(f)
    except Exception as e:
        print(f"Warning: Corrupted registry file ({e}). Resetting.")
        return {}

def save_registry(registry):
    ensure_packages_dir()
    with open(REGISTRY_FILE, 'w') as f:
        json.dump(registry, f, indent=4)

def install_pkg(pkg_name):
    print(f"Installing '{pkg_name}'...")
    registry = load_registry()
    
    if pkg_name in registry:
        print(f"Package '{pkg_name}' is already installed (v{registry[pkg_name]['version']}).")
        # Could check for updates here
        return
        
    # Simulate network fetch
    if pkg_name in MOCK_PUBLIC_REGISTRY:
        pkg_data = MOCK_PUBLIC_REGISTRY[pkg_name]
        
        # Save file
        # Handle "utils/io" -> packages/utils/io.prox
        # Ensure subdirs exist
        parts = pkg_name.split('/')
        if len(parts) > 1:
            subdir = os.path.join(PACKAGES_DIR, *parts[:-1])
            os.makedirs(subdir, exist_ok=True)
            
        filename = os.path.join(PACKAGES_DIR, f"{pkg_name}.prox")
        with open(filename, 'w') as f:
            f.write(pkg_data['content'])
            
        # Update registry
        registry[pkg_name] = {"version": pkg_data['version']}
        save_registry(registry)
        print(f"Successfully installed '{pkg_name}' (v{pkg_data['version']}).")
    else:
        print(f"Error: Package '{pkg_name}' not found in public registry.")

def list_pkgs():
    registry = load_registry()
    if not registry:
        print("No packages installed.")
    else:
        print("Installed Packages:")
        for pkg, meta in registry.items():
            print(f" - {pkg} (v{meta['version']})")

def remove_pkg(pkg_name):
    registry = load_registry()
    if pkg_name not in registry:
        print(f"package '{pkg_name}' not installed.")
        return

    # Remove file
    filename = os.path.join(PACKAGES_DIR, f"{pkg_name}.prox")
    if os.path.exists(filename):
        os.remove(filename)
        # Check if parent dir is empty, remove if so (cleanup)
        directory = os.path.dirname(filename)
        if len(os.listdir(directory)) == 0 and directory != PACKAGES_DIR:
            os.rmdir(directory)
            
    del registry[pkg_name]
    save_registry(registry)
    print(f"Removed '{pkg_name}'.")

def search_pkg(keyword):
    print(f"Searching for '{keyword}'...")
    results = []
    for pkg in MOCK_PUBLIC_REGISTRY:
        if keyword in pkg:
            results.append(pkg)
            
    if results:
        print("Found:")
        for r in results:
            print(f" - {r}")
    else:
        print("No packages found.")

def update_pkg(pkg_name):
    # For now, uninstall and install
    print(f"Updating {pkg_name if pkg_name else 'all'}...")
    if pkg_name:
        install_pkg(pkg_name)
    else:
        # Update all
        registry = load_registry()
        for pkg in registry:
            install_pkg(pkg)

def main():
    parser = argparse.ArgumentParser(description="PRM - ProX Repository Manager")
    subparsers = parser.add_subparsers(dest='command', help='Commands')
    
    # install
    p_install = subparsers.add_parser('install', help='Install a package')
    p_install.add_argument('package', help='Package name (e.g., utils/io)')
    
    # list
    p_list = subparsers.add_parser('list', help='List installed packages')
    
    # remove
    p_remove = subparsers.add_parser('remove', help='Remove a package')
    p_remove.add_argument('package', help='Package name')
    
    # search
    p_search = subparsers.add_parser('search', help='Search for packages')
    p_search.add_argument('keyword', help='Search keyword')

    # update
    p_update = subparsers.add_parser('update', help='Update packages')
    p_update.add_argument('package', nargs='?', help='Specific package to update')

    args = parser.parse_args()
    
    if args.command == 'install':
        install_pkg(args.package)
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

if __name__ == '__main__':
    main()
