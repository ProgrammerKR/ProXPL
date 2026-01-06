
function is_prime(n) {
    if (n <= 1) return false;
    for (let i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

const start = process.hrtime();
let count = 0;
for (let i = 0; i < 5000; i++) {
    if (is_prime(i)) {
        count++;
    }
}
console.log("Primes up to 5000: " + count);
const diff = process.hrtime(start);
const elapsed = diff[0] + diff[1] / 1e9;
console.log("Time: " + elapsed.toFixed(6));
