from stdlib import io, math, string, collections, datetime, system, convert, runtime

class StdLib:
    @staticmethod
    def register(vm):
        io.register(vm)
        math.register(vm)
        string.register(vm)
        collections.register(vm)
        datetime.register(vm)
        system.register(vm)
        convert.register(vm)
        runtime.register(vm)
