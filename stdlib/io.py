import sys

def io_print(args):
    print(*args)
    return None

def io_input(args):
    prompt = args[0] if args else ""
    return input(str(prompt))

def io_read_file(args):
    path = args[0]
    try:
        with open(path, 'r', encoding='utf-8') as f:
            return f.read()
    except Exception as e:
        return None # Or raise Error

def io_write_file(args):
    path = args[0]
    content = args[1]
    try:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(str(content))
        return True
    except:
        return False

def io_append_file(args):
    path = args[0]
    content = args[1]
    try:
        with open(path, 'a', encoding='utf-8') as f:
            f.write(str(content))
        return True
    except:
        return False

def register(vm):
    vm.define_native("print", io_print)
    vm.define_native("input", io_input)
    vm.define_native("read_file", io_read_file)
    vm.define_native("write_file", io_write_file)
    vm.define_native("append_file", io_append_file)
