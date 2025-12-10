import json

def register(vm):
    vm.define_native("to_int", lambda args: int(float(args[0])))
    vm.define_native("to_float", lambda args: float(args[0]))
    vm.define_native("to_string", lambda args: str(args[0]))
    vm.define_native("to_bool", lambda args: bool(args[0]))
    vm.define_native("to_list", lambda args: list(args[0]))
    vm.define_native("to_dict", lambda args: dict(args[0]))
    vm.define_native("to_hex", lambda args: hex(int(args[0])))
    vm.define_native("to_bin", lambda args: bin(int(args[0])))
    vm.define_native("parse_json", lambda args: json.loads(args[0]))
    vm.define_native("stringify_json", lambda args: json.dumps(args[0]))
