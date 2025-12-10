import copy

def register(vm):
    vm.define_native("type", lambda args: type(args[0]).__name__)
    vm.define_native("assert", lambda args: args[0] if args[0] else (_ for _ in ()).throw(AssertionError(args[1] if len(args) > 1 else "Assertion failed")))
    vm.define_native("id", lambda args: id(args[0]))
    vm.define_native("hash", lambda args: hash(args[0]))
    vm.define_native("is_instance", lambda args: isinstance(args[0], args[1])) # Placeholder logic for real class checks
