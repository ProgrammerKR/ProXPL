
function collatz(n) {
    let steps = 0;
    while (n > 1) {
        if (n % 2 == 0) {
            n = Math.floor(n / 2);
        } else {
            n = 3 * n + 1;
        }
        steps++;
    }
    return steps;
}

const start = process.hrtime();
let max_steps = 0;
for (let i = 1; i <= 20000; i++) {
    let steps = collatz(i);
    if (steps > max_steps) max_steps = steps;
}
console.log("Max Collatz steps: " + max_steps);
const diff = process.hrtime(start);
const elapsed = diff[0] + diff[1] / 1e9;
console.log("Time: " + elapsed.toFixed(6));
