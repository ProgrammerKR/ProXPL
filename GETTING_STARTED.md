# Getting Started with ProXPL

Welcome to ProXPL! This guide will walk you through everything you need to start writing programs in ProXPL, from installation to running your first application and managing dependencies.

## Table of Contents

1. [Installation](#step-1-installation)
2. [Your First ProXPL Program](#step-2-your-first-proxpl-program)
3. [Understanding the Basics](#step-3-understanding-the-basics)
4. [Working with the CLI](#step-4-working-with-the-cli)
5. [Using the Package Manager (PRM)](#step-5-using-the-package-manager-prm)
6. [Building a Real Project](#step-6-building-a-real-project)
7. [Next Steps](#next-steps)

---

## Step 1: Installation

ProXPL can be installed in several ways. We'll cover the easiest methods for getting started.

### Option A: Download Pre-built Binary (Recommended for Beginners)

This is the fastest way to get started with ProXPL.

#### Windows

1. Visit the [ProXPL Releases Page](https://github.com/ProgrammerKR/ProXPL/releases/latest)
2. Download `proxpl.exe`
3. Create a folder for ProXPL (e.g., `C:\ProXPL`)
4. Move `proxpl.exe` to this folder
5. Add the folder to your system PATH:
   - Press `Win + X` and select "System"
   - Click "Advanced system settings"
   - Click "Environment Variables"
   - Under "System variables", find and select "Path"
   - Click "Edit" → "New"
   - Add `C:\ProXPL` (or your chosen path)
   - Click "OK" on all dialogs

6. Verify installation:
   ```bash
   # Open a new Command Prompt or PowerShell
   proxpl --version
   ```

#### Linux

1. Visit the [ProXPL Releases Page](https://github.com/ProgrammerKR/ProXPL/releases/latest)
2. Download `proxpl` for Linux
3. Make it executable and move to `/usr/local/bin`:
   ```bash
   chmod +x proxpl
   sudo mv proxpl /usr/local/bin/
   ```

4. Verify installation:
   ```bash
   proxpl --version
   ```

#### macOS

1. Visit the [ProXPL Releases Page](https://github.com/ProgrammerKR/ProXPL/releases/latest)
2. Download `proxpl-macos`
3. Make it executable and move to `/usr/local/bin`:
   ```bash
   chmod +x proxpl-macos
   sudo mv proxpl-macos /usr/local/bin/proxpl
   ```

4. If you get a security warning, go to System Preferences → Security & Privacy and allow the app

5. Verify installation:
   ```bash
   proxpl --version
   ```

### Option B: Build from Source

If you want the latest features or prefer building from source:

**Prerequisites:**
- C/C++ compiler (GCC, Clang, or MSVC)
- CMake 3.15 or higher
- Git

**Build Steps:**

```bash
# Clone the repository
git clone https://github.com/ProgrammerKR/ProXPL.git
cd ProXPL

# Create build directory
mkdir build && cd build

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# The executable will be in the build directory
./proxpl --version

# Optional: Install system-wide
sudo make install
```

### Option C: Install Enhanced CLI (Optional)

For an enhanced developer experience with watch mode and better logging:

**Prerequisites:**
- Node.js 14+ and npm

**Installation:**

```bash
cd ProXPL/src/cli
npm install
npm link
```

Now you can use the `prox` command in addition to `proxpl`:

```bash
prox --version
```

---

## Step 2: Your First ProXPL Program

Let's write and run your first ProXPL program!

### Create a Project Directory

```bash
# Create a directory for your ProXPL projects
mkdir proxpl-projects
cd proxpl-projects
```

### Write "Hello, World!"

Create a file named `hello.prox`:

```javascript
// hello.prox
// My first ProXPL program!

func main() {
    print("Hello, World!");
    print("Welcome to ProXPL programming!");
}

// Execute the main function
main();
```

### Run Your Program

Using the standard ProXPL executable:

```bash
proxpl hello.prox
```

**Expected Output:**
```
Hello, World!
Welcome to ProXPL programming!
```

🎉 **Congratulations!** You just ran your first ProXPL program!

### Using the Enhanced CLI (if installed)

If you installed the Node.js CLI, you can also use:

```bash
prox run hello.prox
```

The enhanced CLI provides colored output and better error messages.

---

## Step 3: Understanding the Basics

Let's explore ProXPL's core features with a more interactive program.

### Working with Variables and Input

Create a file named `greet.prox`:

```javascript
// greet.prox
// Interactive greeting program

func main() {
    // Ask for user's name
    print("╔═══════════════════════════╗");
    print("║  Welcome to ProXPL Demo  ║");
    print("╚═══════════════════════════╝");
    print("");
    
    let name = input("What is your name? ");
    print("Nice to meet you, " + name + "!");
    
    // Ask for favorite number
    let num_str = input("What's your favorite number? ");
    let num = to_int(num_str);
    
    // Perform calculations
    let doubled = num * 2;
    let squared = num * num;
    
    print("\nHere are some fun facts about " + to_string(num) + ":");
    print("- Doubled: " + to_string(doubled));
    print("- Squared: " + to_string(squared));
    
    // Generate a random number
    let lucky = random(1, 100);
    print("- Your lucky number today: " + to_string(lucky));
}

main();
```

### Run the Interactive Program

```bash
proxpl greet.prox
```

**Sample Interaction:**
```
╔═══════════════════════════╗
║  Welcome to ProXPL Demo  ║
╚═══════════════════════════╝

What is your name? Alice
Nice to meet you, Alice!
What's your favorite number? 7

Here are some fun facts about 7:
- Doubled: 14
- Squared: 49
- Your lucky number today: 42
```

### Understanding the Code

Let's break down what we just learned:

1. **Variables**: Use `let` to declare variables
   ```javascript
   let name = "Alice";
   let count = 42;
   ```

2. **Input/Output**: 
   ```javascript
   print("text");        // Output to console
   let x = input("? ");  // Read user input (returns string)
   ```

3. **Type Conversion**:
   ```javascript
   to_int("42")      // String → Integer
   to_string(42)     // Integer → String
   to_float("3.14")  // String → Float
   ```

4. **Standard Library Functions**:
   ```javascript
   random(1, 100)    // Generate random number
   length(list)      // Get collection size
   ```

---

## Step 4: Working with the CLI

ProXPL provides several ways to run your code.

### Direct Execution

Most common way - just run the file:

```bash
proxpl myfile.prox
```

### REPL Mode (Coming Soon)

Interactive mode for testing code snippets:

```bash
prox repl
```

### Watch Mode (Enhanced CLI)

Automatically re-run your program when files change:

```bash
prox run myfile.prox --watch
```

This is great for development - edit your file, save, and see results immediately!

### Getting Help

```bash
# Show version
proxpl --version

# Show help (enhanced CLI)
prox --help
prox run --help
```

---

## Step 5: Using the Package Manager (PRM)

PRM (ProX Repository Manager) helps you manage dependencies and create structured projects.

### Initialize a New Project

```bash
# Create a new project directory
mkdir my-app
cd my-app

# Initialize with PRM
prm init
```

This creates:
- `prox.toml` - Project manifest file
- `src/main.prox` - Entry point for your application
- `.gitignore` - Git ignore file

### Understanding `prox.toml`

After running `prm init`, you'll see a `prox.toml` file:

```toml
[package]
name = "my-app"
version = "0.1.0"
authors = ["Your Name <you@example.com>"]
edition = "2025"
description = "A ProXPL application"
license = "MIT"

[dependencies]
# Add your dependencies here

[build]
target = "bytecode"
optimize = false
```

### Creating Your Main File

Edit `src/main.prox`:

```javascript
// src/main.prox
// Main entry point for my-app

func main() {
    print("My ProXPL Application v0.1.0");
    print("============================");
    
    // Your application logic here
    let result = calculate_something();
    print("Result: " + to_string(result));
}

func calculate_something() {
    // Some calculation
    return 42 * 2;
}

main();
```

### Running Your Project

From the project root directory:

```bash
# Run using PRM
prm run

# Or run the main file directly
proxpl src/main.prox
```

### Adding Dependencies

Let's say you want to add a JSON parsing library (when available):

```bash
# Add a dependency
prm add json
```

This will:
1. Download the package
2. Add it to `prox.toml`
3. Create/update `prox.lock` (lockfile)

Your `prox.toml` will be updated:

```toml
[dependencies]
json = "1.0.0"
```

### Using Dependencies in Your Code

```javascript
// Import the dependency
use json;

func main() {
    // Use the imported module
    let data = json.parse("{\"name\": \"ProXPL\"}");
    print(data["name"]);
}
```

### Other PRM Commands

```bash
# List all installed packages
prm list

# Search for packages
prm search http

# Update dependencies
prm update

# Remove a dependency
prm remove json

# Show package info
prm info json
```

---

## Step 6: Building a Real Project

Let's build a simple calculator application that demonstrates multiple concepts.

### Project Setup

```bash
mkdir calculator-app
cd calculator-app
prm init
```

### Create the Calculator

Edit `src/main.prox`:

```javascript
// src/main.prox
// Simple Calculator Application

func main() {
    print("╔══════════════════════════╗");
    print("║  ProXPL Calculator v1.0  ║");
    print("╚══════════════════════════╝");
    print("");
    
    let running = true;
    
    while (running) {
        print("\nOperations:");
        print("1. Add");
        print("2. Subtract");
        print("3. Multiply");
        print("4. Divide");
        print("5. Power");
        print("6. Square Root");
        print("0. Exit");
        
        let choice = input("\nEnter choice: ");
        
        if (choice == "0") {
            running = false;
            print("Thank you for using ProXPL Calculator!");
        } else if (choice == "1") {
            perform_operation("add");
        } else if (choice == "2") {
            perform_operation("subtract");
        } else if (choice == "3") {
            perform_operation("multiply");
        } else if (choice == "4") {
            perform_operation("divide");
        } else if (choice == "5") {
            perform_operation("power");
        } else if (choice == "6") {
            perform_sqrt();
        } else {
            print("Invalid choice! Please try again.");
        }
    }
}

func perform_operation(op) {
    let a = to_float(input("Enter first number: "));
    let b = to_float(input("Enter second number: "));
    
    let result = 0.0;
    
    if (op == "add") {
        result = a + b;
        print("Result: " + to_string(a) + " + " + to_string(b) + " = " + to_string(result));
    } else if (op == "subtract") {
        result = a - b;
        print("Result: " + to_string(a) + " - " + to_string(b) + " = " + to_string(result));
    } else if (op == "multiply") {
        result = a * b;
        print("Result: " + to_string(a) + " × " + to_string(b) + " = " + to_string(result));
    } else if (op == "divide") {
        if (b == 0.0) {
            print("Error: Cannot divide by zero!");
        } else {
            result = a / b;
            print("Result: " + to_string(a) + " ÷ " + to_string(b) + " = " + to_string(result));
        }
    } else if (op == "power") {
        result = pow(a, b);
        print("Result: " + to_string(a) + " ^ " + to_string(b) + " = " + to_string(result));
    }
}

func perform_sqrt() {
    let num = to_float(input("Enter a number: "));
    
    if (num < 0.0) {
        print("Error: Cannot calculate square root of negative number!");
    } else {
        let result = sqrt(num);
        print("Result: √" + to_string(num) + " = " + to_string(result));
    }
}

main();
```

### Run Your Calculator

```bash
prm run
```

Or:

```bash
proxpl src/main.prox
```

### Test It Out

```
╔══════════════════════════╗
║  ProXPL Calculator v1.0  ║
╚══════════════════════════╝

Operations:
1. Add
2. Subtract
3. Multiply
4. Divide
5. Power
6. Square Root
0. Exit

Enter choice: 5
Enter first number: 2
Enter second number: 8
Result: 2 ^ 8 = 256
```

---

## Next Steps

Congratulations! You now know the basics of ProXPL. Here's what to explore next:

### 📚 Learn More

1. **[Language Specification](docs/language-spec/)** - Complete language reference
2. **[Standard Library](docs/stdlib/)** - All 75+ built-in functions
3. **[Examples](examples/)** - More example programs
4. **[Architecture Guide](docs/architecture/)** - How ProXPL works internally

### 💡 Try These Challenges

1. **Fibonacci Generator**: Write a program that prints the first N Fibonacci numbers
2. **File Reader**: Use `read_file()` and `write_file()` to process text files
3. **List Processor**: Work with lists using `push()`, `pop()`, `length()`
4. **Dictionary Manager**: Create a simple contact book using dictionaries

### 🔧 Advanced Topics (When Ready)

- **Module System**: Create reusable modules with `use`
- **Type System**: Understanding static typing and type inference
- **Performance**: Using the LLVM backend for native compilation
- **Debugging**: Using the LSP and debugger tools

### 🌟 Example Challenges

#### Challenge 1: FizzBuzz

Write a program that prints numbers 1-100, but:
- Print "Fizz" for multiples of 3
- Print "Buzz" for multiples of 5
- Print "FizzBuzz" for multiples of both

<details>
<summary>Click to see solution</summary>

```javascript
func fizzbuzz() {
    for (let i = 1; i <= 100; i = i + 1) {
        if (i % 15 == 0) {
            print("FizzBuzz");
        } else if (i % 3 == 0) {
            print("Fizz");
        } else if (i % 5 == 0) {
            print("Buzz");
        } else {
            print(to_string(i));
        }
    }
}

fizzbuzz();
```
</details>

#### Challenge 2: Palindrome Checker

Check if a string is a palindrome.

<details>
<summary>Click to see solution</summary>

```javascript
func is_palindrome(text) {
    let reversed = reverse_string(text);
    return text == reversed;
}

func reverse_string(s) {
    let result = "";
    let len = length(s);
    for (let i = len - 1; i >= 0; i = i - 1) {
        result = result + s[i];
    }
    return result;
}

func main() {
    let word = input("Enter a word: ");
    if (is_palindrome(word)) {
        print("'" + word + "' is a palindrome!");
    } else {
        print("'" + word + "' is not a palindrome.");
    }
}

main();
```
</details>

---

## Getting Help

If you run into issues:

1. **Check the docs**: Most questions are answered in [docs/](docs/)
2. **Browse examples**: See [examples/](examples/) for working code
3. **Ask the community**: [GitHub Discussions](https://github.com/ProgrammerKR/ProXPL/discussions)
4. **Report bugs**: [GitHub Issues](https://github.com/ProgrammerKR/ProXPL/issues)

## Common Issues

### "proxpl: command not found"

**Solution**: Make sure ProXPL is in your system PATH. On Linux/Mac, try:
```bash
which proxpl
```

If nothing is returned, reinstall or add the directory to PATH.

### "Error: Cannot open file"

**Solution**: Make sure you're running the command from the correct directory:
```bash
# Check current directory
pwd

# List files
ls

# Run from correct location
cd /path/to/your/project
proxpl myfile.prox
```

### "Syntax Error at line X"

**Solution**: ProXPL syntax is strict. Common issues:
- Missing semicolons `;` at end of statements
- Mismatched braces `{}`
- Using `=` instead of `==` in conditions
- Forgetting `let` when declaring variables

---

## Quick Reference Card

### Basic Syntax

```javascript
// Comments
// This is a single-line comment

// Variables
let name = "Alice";
let age = 25;
let active = true;

// Functions
func greet(name) {
    return "Hello, " + name;
}

// Conditionals
if (age >= 18) {
    print("Adult");
} else {
    print("Minor");
}

// Loops
for (let i = 0; i < 10; i = i + 1) {
    print(to_string(i));
}

while (active) {
    // Do something
}

// Collections
let list = [1, 2, 3];
let dict = {"key": "value"};
```

### Common Functions

```javascript
// I/O
print("text")           // Print to console
input("prompt")         // Get user input

// Type Conversion
to_string(42)           // → "42"
to_int("42")            // → 42
to_float("3.14")        // → 3.14

// Math
random(1, 100)          // Random integer
sqrt(16)                // Square root → 4
pow(2, 8)               // Power → 256
abs(-5)                 // Absolute value → 5

// Strings
length("hello")         // → 5
to_upper("hello")       // → "HELLO"
to_lower("HELLO")       // → "hello"

// Lists
push(list, item)        // Add to end
pop(list)               // Remove from end
length(list)            // Get size
```

---

<p align="center">
  <b>Happy Coding with ProXPL! 🚀</b><br>
  <i>Build something amazing!</i>
</p>
