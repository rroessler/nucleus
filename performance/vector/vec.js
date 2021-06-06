/** Computes a vector normalization for a 3D vector. */
function vecNorm(x, y, z) {
    let mag = Math.sqrt(x * x + y * y + z * z);
    let scale = 0;
    if (mag != 0) scale = 1 / mag;
}

/** JavaScript Benchaming method */
const bench = iters => {
    let min = Infinity;
    let max = 0;
    let sum = 0n;

    for (let i = 0; i < iters; i++) {
        const t_start = process.hrtime.bigint();
        for (let j = 0; j < 100000; j++)  vecNorm(2, 8, 2);
        const t_duration = (process.hrtime.bigint() - t_start) / 1000n;

        sum += t_duration;
        if (t_duration < min) min = t_duration;
        if (t_duration > max) max = t_duration;
    }

    console.log(`Average: ${sum / BigInt(iters)}us`);
    console.log(`Min: ${min}us`);
    console.log(`Max: ${max}us`);
}

console.log('\n=> JavaScript');
bench(75);
console.log();