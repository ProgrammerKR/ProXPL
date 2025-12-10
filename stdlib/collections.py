import copy

def register(vm):
    vm.define_native("range", lambda args: list(range(int(args[0]), int(args[1]), int(args[2]) if len(args) > 2 else 1)))
    vm.define_native("push", lambda args: args[0].append(args[1]) or args[0])
    vm.define_native("pop", lambda args: args[0].pop())
    vm.define_native("insert", lambda args: args[0].insert(int(args[1]), args[2]) or args[0])
    vm.define_native("remove", lambda args: args[0].remove(args[1]) or args[0])
    vm.define_native("sort", lambda args: sorted(args[0]))
    vm.define_native("reverse", lambda args: list(reversed(args[0])))
    vm.define_native("keys", lambda args: list(args[0].keys()))
    vm.define_native("values", lambda args: list(args[0].values()))
    vm.define_native("entries", lambda args: list(args[0].items()))
    vm.define_native("contains_key", lambda args: args[1] in args[0])
    vm.define_native("merge", lambda args: {**args[0], **args[1]})
    vm.define_native("clone", lambda args: args[0].copy())
    vm.define_native("deep_clone", lambda args: copy.deepcopy(args[0]))
    vm.define_native("clear", lambda args: args[0].clear())
