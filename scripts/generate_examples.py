import os

examples = {
    "examples/advanced/iop_event_bus.prox": """// ProXPL Intent-Oriented Event Bus
// Demonstrates decoupling components using intents.

intent LogEvent(message: string, level: string);
intent ProcessData(payload: dict);

resolver HandleLog(matches LogEvent(m, l)) {
    print("[" + to_upper(l) + "] " + m);
}

resolver HandleData(matches ProcessData(p)) {
    LogEvent("Processing data for user: " + p["user"], "info");
    // Complex processing here
    LogEvent("Data processed successfully.", "success");
}

func main() {
    print("Starting IOP Event Bus Simulation...");
    ProcessData({"user": "admin", "action": "login"});
}
main();
""",
    "examples/advanced/context_auth_layer.prox": """// Context-Aware Polymorphism Authentication Simulation
// Overrides database fetch behavior based on active authentication context.

func fetch_record(id) {
    return "Encrypted Record " + to_string(id);
}

layer AuthenticatedAdmin {
    override func fetch_record(id) {
        return "Decrypted Sensitive Record " + to_string(id);
    }
}

func main() {
    print("Default Context: " + fetch_record(42));
    
    activate AuthenticatedAdmin {
        print("Admin Context: " + fetch_record(42));
    }
    
    print("Back to Default: " + fetch_record(42));
}
main();
""",
    "examples/advanced/asr_db_transaction.prox": """// Autonomic Self-Healing (ASR) Database Transaction
// Demonstrates safe unwinding using resilient blocks.

func write_to_db(data) {
    if (data == null) {
        throw "NullDataException: Cannot write null to DB";
    }
    print("Writing " + data + " to database.");
}

func main() {
    print("Starting transaction...");
    
    resilient {
        write_to_db("User record 1");
        write_to_db(null); // This will throw
        write_to_db("User record 2"); // Will be skipped
    } recovery (err) {
        print("Transaction failed. Rolling back due to: " + err);
    }
    
    print("System continues operating normally.");
}
main();
""",
    "examples/advanced/taint_security_check.prox": """// Security Taint Analysis (Simulated)
// Demonstrates the Intrinsic Security pillar.

func sanitize_input(input_str) {
    print("Sanitizing input...");
    return input_str; // Removes malicious characters
}

func execute_query(query) {
    print("Executing query: " + query);
}

func main() {
    let tainted_user_input = "DROP TABLE users; --";
    
    // In ProXPL, this would natively throw a TaintedValueError
    // execute_query(tainted_user_input); 
    
    let safe_input = sanitize_input(tainted_user_input);
    execute_query(safe_input);
}
main();
""",
    "examples/algorithms/binary_tree.prox": """// Binary Search Tree implementation

class Node {
    let value: int;
    let left: any;
    let right: any;
    
    init(val) {
        this.value = val;
        this.left = null;
        this.right = null;
    }
}

class BST {
    let root: any;
    
    init() {
        this.root = null;
    }
    
    func insert(val) {
        if (this.root == null) {
            this.root = new Node(val);
            return;
        }
        
        let current = this.root;
        while (true) {
            if (val < current.value) {
                if (current.left == null) {
                    current.left = new Node(val);
                    return;
                }
                current = current.left;
            } else {
                if (current.right == null) {
                    current.right = new Node(val);
                    return;
                }
                current = current.right;
            }
        }
    }
}

func main() {
    let tree = new BST();
    tree.insert(10);
    tree.insert(5);
    tree.insert(15);
    print("BST populated.");
}
main();
""",
    "examples/algorithms/graph_bfs.prox": """// Breadth-First Search on a Graph using Lists

func bfs(graph, start) {
    let visited = [];
    let queue = [start];
    push(visited, start);
    
    while (length(queue) > 0) {
        let node = queue[0];
        // Pop front
        let new_q = [];
        for (let i = 1; i < length(queue); i = i + 1) {
            push(new_q, queue[i]);
        }
        queue = new_q;
        
        print("Visiting: " + node);
        
        let neighbors = graph[node];
        if (neighbors != null) {
            for (let i = 0; i < length(neighbors); i = i + 1) {
                let n = neighbors[i];
                let found = false;
                for (let j = 0; j < length(visited); j = j + 1) {
                    if (visited[j] == n) {
                        found = true;
                    }
                }
                if (!found) {
                    push(visited, n);
                    push(queue, n);
                }
            }
        }
    }
}

func main() {
    let graph = {
        "A": ["B", "C"],
        "B": ["D", "E"],
        "C": ["F"],
        "D": [],
        "E": ["F"],
        "F": []
    };
    
    bfs(graph, "A");
}
main();
""",
    "examples/algorithms/lru_cache.prox": """// Simple LRU Cache implementation using Lists and Dicts

class LRUCache {
    let capacity: int;
    let cache: dict;
    let order: any; // List to track usage
    
    init(cap) {
        this.capacity = cap;
        this.cache = {};
        this.order = [];
    }
    
    func get(key) {
        if (this.cache[key] != null) {
            // Update order (move to front)
            this._refresh(key);
            return this.cache[key];
        }
        return -1;
    }
    
    func put(key, value) {
        if (this.cache[key] == null) {
            if (length(this.order) >= this.capacity) {
                let lru_key = this.order[length(this.order) - 1];
                this.cache[lru_key] = null; // Evict
                // Remove from order (simplified)
            }
            this.cache[key] = value;
            // Push front (simplified)
            push(this.order, key);
        } else {
            this.cache[key] = value;
            this._refresh(key);
        }
    }
    
    func _refresh(key) {
        print("Refreshing key: " + key);
    }
}

func main() {
    let lru = new LRUCache(2);
    lru.put("A", 1);
    lru.put("B", 2);
    print("A: " + to_string(lru.get("A"))); // 1
    lru.put("C", 3); // Evicts B
    print("B: " + to_string(lru.get("B"))); // -1
}
main();
""",
    "examples/math/neural_net_forward.prox": """// AI-Native Pillar: Neural Network Forward Pass Simulation

func relu(x) {
    if (x > 0) return x;
    return 0;
}

func main() {
    print("Initializing weights and biases...");
    
    // Simplistic mock of tensor operations
    let input = [1.0, -0.5, 2.0];
    let weights = [0.5, 1.5, -1.0];
    let bias = 0.1;
    
    let sum = 0.0;
    for (let i = 0; i < length(input); i = i + 1) {
        sum = sum + (input[i] * weights[i]);
    }
    sum = sum + bias;
    
    let activation = relu(sum);
    
    print("Network Output: " + to_string(activation));
}
main();
""",
    "examples/math/matrix_transform.prox": """// Basic Matrix transformations using arrays

func transpose(matrix) {
    let rows = length(matrix);
    let cols = length(matrix[0]);
    
    let result = [];
    for (let i = 0; i < cols; i = i + 1) {
        let new_row = [];
        for (let j = 0; j < rows; j = j + 1) {
            push(new_row, matrix[j][i]);
        }
        push(result, new_row);
    }
    return result;
}

func main() {
    let mat = [
        [1, 2, 3],
        [4, 5, 6]
    ];
    
    print("Original:");
    print(mat);
    
    let transposed = transpose(mat);
    
    print("Transposed:");
    print(transposed);
}
main();
""",
    "examples/math/statistical_analysis.prox": """// Statistical Math operations

func calculate_mean(data) {
    let sum = 0.0;
    let n = length(data);
    if (n == 0) return 0;
    
    for (let i = 0; i < n; i = i + 1) {
        sum = sum + data[i];
    }
    return sum / n;
}

func main() {
    let dataset = [12.5, 45.2, 33.8, 19.1, 7.6, 92.4];
    
    let mean = calculate_mean(dataset);
    
    print("Dataset elements: " + to_string(length(dataset)));
    print("Mean Value: " + to_string(mean));
}
main();
""",
    "examples/ui_and_web/rest_api_mock.prox": """// Mock REST API Server routing using Intent-Oriented Programming

intent RouteGet(path: string);
intent RoutePost(path: string, body: dict);

resolver GetUsers(matches RouteGet("/users")) {
    return {"status": 200, "data": ["Alice", "Bob", "Charlie"]};
}

resolver PostUser(matches RoutePost("/users", body)) {
    return {"status": 201, "message": "User created", "user": body["name"]};
}

resolver NotFound(matches RouteGet(path)) {
    return {"status": 404, "message": "Route not found: " + path};
}

func main() {
    print("Simulating GET /users...");
    let get_res = RouteGet("/users");
    print(get_res);
    
    print("Simulating POST /users...");
    let post_res = RoutePost("/users", {"name": "Dave"});
    print(post_res);
    
    print("Simulating GET /invalid...");
    let 404_res = RouteGet("/invalid");
    print(404_res);
}
main();
""",
    "examples/ui_and_web/middleware_pipeline.prox": """// Web Middleware Pipeline implementation

class Request {
    let url: string;
    let auth: string;
    
    init(url) {
        this.url = url;
        this.auth = null;
    }
}

func auth_middleware(req) {
    if (req.url == "/admin") {
        if (req.auth != "secret-token") {
            throw "Unauthorized";
        }
    }
    return true;
}

func logging_middleware(req) {
    print("[LOG] Incoming request to " + req.url);
    return true;
}

func main() {
    let req = new Request("/admin");
    
    resilient {
        logging_middleware(req);
        auth_middleware(req);
        print("Access Granted to " + req.url);
    } recovery (e) {
        print("403 Forbidden: " + e);
    }
    
    req.auth = "secret-token";
    resilient {
        logging_middleware(req);
        auth_middleware(req);
        print("Access Granted to " + req.url);
    } recovery (e) {
        print("403 Forbidden: " + e);
    }
}
main();
""",
    "examples/systems/config_parser.prox": """// Simple Configuration Parser Simulator

func parse_ini(content) {
    print("Parsing INI configuration...");
    let lines = split(content, "\\n");
    let config = {};
    let current_section = "global";
    
    for (let i = 0; i < length(lines); i = i + 1) {
        let line = lines[i];
        // Extremely simplified parsing logic
        if (line != "") {
            config[line] = true;
        }
    }
    return config;
}

func main() {
    let raw = "host=localhost\\nport=8080\\ndebug=true";
    let conf = parse_ini(raw);
    print("Parsed Configuration keys:");
    print(conf);
}
main();
""",
    "examples/systems/log_rotator.prox": """// Systems Programming: Log Rotator Simulation

func rotate_logs(max_size) {
    let current_size = random(0, 100);
    
    print("Checking log size... (" + to_string(current_size) + "MB)");
    if (current_size > max_size) {
        print("Rotating log file. Archiving old logs...");
        return true;
    }
    print("Log size within limits.");
    return false;
}

func main() {
    for (let i = 0; i < 5; i = i + 1) {
        rotate_logs(50);
    }
}
main();
""",
    "examples/systems/process_monitor.prox": """// Process Monitor Health Check Simulation

intent CheckHealth(pid: int);

resolver HealthyProcess(matches CheckHealth(pid)) {
    if (pid % 2 == 0) {
        return "UP";
    }
    throw "ProcessDownException";
}

func monitor_process(pid) {
    resilient {
        let status = CheckHealth(pid);
        print("Process " + to_string(pid) + " is " + status);
    } recovery (err) {
        print("Process " + to_string(pid) + " requires restart. (" + err + ")");
    }
}

func main() {
    monitor_process(100);
    monitor_process(101);
}
main();
""",
    "examples/basics/string_manipulation.prox": """// String operations in ProXPL

func main() {
    let message = "ProXPL is elegant and fast";
    print("Original: " + message);
    
    // Built-in functions
    print("Upper: " + to_upper(message));
    print("Lower: " + to_lower(message));
    
    let words = split(message, " ");
    print("Word count: " + to_string(length(words)));
    
    let replaced = replace(message, "fast", "powerful");
    print("Replaced: " + replaced);
}
main();
""",
    "examples/basics/date_time_calc.prox": """// Date Time Calculation simulation

func main() {
    // Simulated time functions since stdlib time varies
    let start = time();
    
    print("Performing heavy computation...");
    let sum = 0;
    for (let i = 0; i < 10000; i = i + 1) {
        sum = sum + i;
    }
    
    let end = time();
    print("Computation finished.");
    
    // Some timestamps
    print("Start Epoch: " + to_string(start));
    print("End Epoch: " + to_string(end));
}
main();
""",
    "examples/basics/error_handling_101.prox": """// Traditional Error Handling in ProXPL

func risky_function(should_fail) {
    if (should_fail) {
        throw "RuntimeError: Operation failed!";
    }
    return "Success!";
}

func main() {
    print("Attempting risky operations...");
    
    resilient {
        print(risky_function(false));
        print(risky_function(true));
        print("This will not be printed");
    } recovery (e) {
        print("Caught exception gracefully: " + e);
    }
}
main();
""",
    "examples/basics/functional_map_reduce.prox": """// Functional Programming Patterns (Map/Reduce)

// Note: ProXPL v1.6 supports passing closures natively!

func map(array, fn) {
    let result = [];
    for (let i = 0; i < length(array); i = i + 1) {
        push(result, fn(array[i]));
    }
    return result;
}

func reduce(array, fn, initial) {
    let accumulator = initial;
    for (let i = 0; i < length(array); i = i + 1) {
        accumulator = fn(accumulator, array[i]);
    }
    return accumulator;
}

func main() {
    let numbers = [1, 2, 3, 4, 5];
    
    // Closure syntax
    let doubled = map(numbers, func(x) { return x * 2; });
    print("Doubled:");
    print(doubled);
    
    let sum = reduce(numbers, func(acc, val) { return acc + val; }, 0);
    print("Sum:");
    print(to_string(sum));
}
main();
"""
}

for path, content in examples.items():
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        f.write(content)

print(f"Generated {len(examples)} examples.")
