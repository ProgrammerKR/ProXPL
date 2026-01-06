function main() {
    const start = process.hrtime();
    let dict = {};
    for (let i = 0; i < 10000; i++) {
        dict["key" + i] = i;
    }

    let sum = 0;
    for (let i = 0; i < 10000; i++) {
        sum += dict["key" + i];
    }

    console.log("Dict Sum: " + sum);
    const diff = process.hrtime(start);
    const elapsed = diff[0] + diff[1] / 1e9;
    console.log(`Time: ${elapsed.toFixed(6)}`);
}

main();
