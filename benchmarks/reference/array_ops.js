function main() {
    const start = process.hrtime();
    let arr = [];
    for (let i = 0; i < 50000; i++) {
        arr.push(i);
    }

    let sum = 0;
    for (let i = 0; i < arr.length; i++) {
        sum += arr[i];
    }

    // Pop half
    for (let i = 0; i < 25000; i++) {
        arr.pop();
    }

    console.log("Array Sum: " + sum);
    console.log("Array Length: " + arr.length);
    const diff = process.hrtime(start);
    const elapsed = diff[0] + diff[1] / 1e9;
    console.log(`Time: ${elapsed.toFixed(6)}`);
}

main();
