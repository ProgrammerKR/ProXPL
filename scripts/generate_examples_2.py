import os

examples = {
    "examples/ui_and_web/html_templater.prox": """// Very simple HTML template engine simulator

func render(template, data) {
    let result = template;
    // Simulating replace loop (assuming a powerful replace function exists)
    // For ProXPL v1.6, we'd use native string manipulation
    let name = data["name"];
    let title = data["title"];
    
    result = replace(result, "{{name}}", name);
    result = replace(result, "{{title}}", title);
    return result;
}

func main() {
    let tpl = "<html><head><title>{{title}}</title></head><body><h1>Hello, {{name}}!</h1></body></html>";
    let output = render(tpl, {"name": "Alice", "title": "Welcome Page"});
    print(output);
}
main();
""",
    "examples/basics/file_parsing.prox": """// Basic file parsing simulation

func parse_csv_line(line) {
    return split(line, ",");
}

func main() {
    let csv = "id,name,role\\n1,Alice,Admin\\n2,Bob,User";
    let lines = split(csv, "\\n");
    
    print("Parsed CSV Data:");
    for (let i = 0; i < length(lines); i = i + 1) {
        let cols = parse_csv_line(lines[i]);
        print(cols);
    }
}
main();
""",
    "examples/math/complex_numbers.prox": """// Object-Oriented Complex Numbers

class Complex {
    let real: float;
    let imag: float;
    
    init(r, i) {
        this.real = r;
        this.imag = i;
    }
    
    func add(other) {
        return new Complex(this.real + other.real, this.imag + other.imag);
    }
    
    func to_string() {
        if (this.imag >= 0) {
            return to_string(this.real) + " + " + to_string(this.imag) + "i";
        }
        return to_string(this.real) + " - " + to_string(-this.imag) + "i";
    }
}

func main() {
    let a = new Complex(3.5, 2.0);
    let b = new Complex(1.5, -4.0);
    
    let c = a.add(b);
    print("Complex A: " + a.to_string());
    print("Complex B: " + b.to_string());
    print("Sum: " + c.to_string());
}
main();
"""
}

for path, content in examples.items():
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        f.write(content)

print(f"Generated {len(examples)} more examples.")
