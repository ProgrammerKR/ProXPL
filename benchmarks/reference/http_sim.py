
import time

class Request:
    def __init__(self, method, path):
        self.method = method
        self.path = path

class Router:
    def __init__(self):
        self.routes = {}

    def add(self, method, path, handler):
        key = method + ":" + path
        self.routes[key] = handler

    def dispatch(self, req):
        key = req.method + ":" + req.path
        if key in self.routes:
            return self.routes[key]()
        return "404 Not Found"

def home(): return "Welcome Home"
def users(): return "User List"
def about(): return "About Us"

def main():
    router = Router()
    router.add("GET", "/", home)
    router.add("GET", "/users", users)
    router.add("GET", "/about", about)

    start = time.time()
    for i in range(50000):
        r1 = Request("GET", "/")
        router.dispatch(r1)

        r2 = Request("GET", "/users")
        router.dispatch(r2)

        r3 = Request("GET", "/missing")
        router.dispatch(r3)
    
    elapsed = time.time() - start
    print(f"HTTP Sim Time: {elapsed:.6f}")

if __name__ == "__main__":
    main()
