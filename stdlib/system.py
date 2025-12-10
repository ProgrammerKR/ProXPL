import sys
import platform
import subprocess

def register(vm):
    vm.define_native("exit", lambda args: sys.exit(int(args[0] if args else 0)))
    vm.define_native("env", lambda args: __import__('os').getenv(args[0]))
    vm.define_native("platform", lambda args: platform.system())
    vm.define_native("version", lambda args: "ProXPL 1.0.0")
    vm.define_native("exec", lambda args: subprocess.run(args[0], shell=True, capture_output=True).stdout.decode())
