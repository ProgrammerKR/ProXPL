
import time
from datetime import datetime

def register(vm):
    vm.define_native("now", lambda args: time.time())
    vm.define_native("timestamp", lambda args: int(time.time()))
    vm.define_native("format_date", lambda args: datetime.fromtimestamp(args[0]).strftime(args[1]))
    vm.define_native("parse_date", lambda args: datetime.strptime(args[0], args[1]).timestamp())
    vm.define_native("sleep", lambda args: time.sleep(args[0]) or True)
