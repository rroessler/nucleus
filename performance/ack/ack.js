/** JavaScipt Ackermann Function */
const ack = (m, n) => {
    if (m == 0) return n + 1;
    if ((m > 0) && (n == 0)) return ack(m - 1, 1);
    if ((m > 0) && (n > 0)) return ack(m - 1, ack(m, n - 1));
}

/** JavaScript Benchaming method */
const bench = iters => {
    let min = Infinity;
    let max = 0;
    let sum = 0n;

    for (let i = 0; i < iters; i++) {
        const t_start = process.hrtime.bigint();
        ack(3, 6);
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
bench(100);
console.log();
