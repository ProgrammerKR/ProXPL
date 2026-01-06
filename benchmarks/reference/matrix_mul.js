
const start = process.hrtime();
const size = 50;
let a = [];
let b = [];

for (let i = 0; i < size; i++) {
    let row_a = [];
    let row_b = [];
    for (let j = 0; j < size; j++) {
        row_a.push(i + j);
        row_b.push(i - j);
    }
    a.push(row_a);
    b.push(row_b);
}

let c = [];
for (let i = 0; i < size; i++) {
    let row_c = [];
    for (let j = 0; j < size; j++) {
        let sum = 0;
        for (let k = 0; k < size; k++) {
            sum += a[i][k] * b[k][j];
        }
        row_c.push(sum);
    }
    c.push(row_c);
}

console.log("Matrix[0][0]: " + c[0][0]);
const diff = process.hrtime(start);
const elapsed = diff[0] + diff[1] / 1e9;
console.log("Time: " + elapsed.toFixed(6));
