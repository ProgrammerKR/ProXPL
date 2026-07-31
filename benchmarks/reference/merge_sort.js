
function merge(left, right) {
    let result = [];
    let i = 0, j = 0;
    while(i < left.length && j < right.length) {
        if(left[i] <= right[j]) {
            result.push(left[i++]);
        } else {
            result.push(right[j++]);
        }
    }
    while(i < left.length) result.push(left[i++]);
    while(j < right.length) result.push(right[j++]);
    return result;
}

function mergeSort(arr) {
    if(arr.length <= 1) return arr;
    let mid = Math.floor(arr.length / 2);
    let left = arr.slice(0, mid);
    let right = arr.slice(mid);
    return merge(mergeSort(left), mergeSort(right));
}

let start = performance.now();
let arr = [];
for(let i = 0; i < 5000; i++) arr.push(5000 - i);
let sorted = mergeSort(arr);
let elapsed = (performance.now() - start) / 1000.0;

console.log("First: " + sorted[0]);
console.log("Last: " + sorted[sorted.length - 1]);
console.log("Time: " + elapsed.toFixed(6));
