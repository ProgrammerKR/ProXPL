
function fib(n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}

const start = process.hrtime();
console.log("Fibonacci(30): " + fib(30));
const diff = process.hrtime(start);
const elapsed = diff[0] + diff[1] / 1e9;
console.log("Time: " + elapsed.toFixed(6));
