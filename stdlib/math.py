
import math as m
import random as r

def register(vm):
    vm.define_native("abs", lambda args: abs(args[0]))
    vm.define_native("ceil", lambda args: m.ceil(args[0]))
    vm.define_native("floor", lambda args: m.floor(args[0]))
    vm.define_native("round", lambda args: round(args[0], args[1] if len(args) > 1 else 0))
    vm.define_native("max", lambda args: max(args))
    vm.define_native("min", lambda args: min(args))
    vm.define_native("pow", lambda args: m.pow(args[0], args[1]))
    vm.define_native("sqrt", lambda args: m.sqrt(args[0]))
    vm.define_native("sin", lambda args: m.sin(args[0]))
    vm.define_native("cos", lambda args: m.cos(args[0]))
    vm.define_native("tan", lambda args: m.tan(args[0]))
    vm.define_native("log", lambda args: m.log(args[0], args[1] if len(args) > 1 else m.e))
    vm.define_native("exp", lambda args: m.exp(args[0]))
    vm.define_native("random", lambda args: r.random())
    vm.define_native("randint", lambda args: r.randint(args[0], args[1]))
