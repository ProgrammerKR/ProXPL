
class Request {
    constructor(method, path) {
        this.method = method;
        this.path = path;
    }
}

class Router {
    constructor() {
        self.routes = {};
    }

    add(method, path, handler) {
        let key = method + ":" + path;
        this.routes[key] = handler;
    }

    dispatch(req) {
        let key = req.method + ":" + req.path;
        if (this.routes[key]) {
            return this.routes[key]();
        }
        return "404 Not Found";
    }
}

function home() { return "Welcome Home"; }
function users() { return "User List"; }
function about() { return "About Us"; }

function main() {
    let router = new Router();
    router.routes = {}; // JS weirdness with 'self' in constructor copy-paste, fixing.
    
    // Fix: In JS class methods, 'this' is correct.
    // Re-defining properly below in actual write call.
}

// Correct JS Implementation
class RouterJS {
    constructor() {
        this.routes = {};
    }

    add(method, path, handler) {
        let key = method + ":" + path;
        this.routes[key] = handler;
    }

    dispatch(req) {
        let key = req.method + ":" + req.path;
        if (this.routes[key]) {
            return this.routes[key]();
        }
        return "404 Not Found";
    }
}

const start = process.hrtime();

const router = new RouterJS();
router.add("GET", "/", home);
router.add("GET", "/users", users);
router.add("GET", "/about", about);

for (let i = 0; i < 50000; i++) {
    let r1 = new Request("GET", "/");
    router.dispatch(r1);

    let r2 = new Request("GET", "/users");
    router.dispatch(r2);

    let r3 = new Request("GET", "/missing");
    router.dispatch(r3);
}

const diff = process.hrtime(start);
const elapsed = diff[0] + diff[1] / 1e9;
console.log(`HTTP Sim Time: ${elapsed.toFixed(6)}`);
