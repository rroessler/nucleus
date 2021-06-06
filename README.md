<div align="center">

Nucleus
=======

---

<img width="100px" src="./assets/nucleus-logo.svg">

Nucleus is a dynamic, bytecode driven programming language.

---
    
</div>

Features
--------

- Lightness: The binary currently compiles to under 150kB.
- Easily Accessible: Plans will be to integrate Nucleus via WASM and a Node.js run CLI.

Primary Goals
-------------

- Reliability: Nucleus aims to work consistently and expectedly.
- Dynamic Stability: Better interoperability between dynamic types is a must.
- Rich Native Library: For faster and efficient programs, native reaction calls in C are a must.

Quick Start
-----------

At this stage the Nucleus project can only be compiled from source. The only tool required to do this currenty do this is either "gcc" or "clang". Other setups have not been tested, however "MinGW" and "MSCV" should work. Additionally Git will be required to clone this repo.

```bash
git clone https://github.com/rroessler/nucleus.git
cd nucleus
gcc -O3 -g "./src/nucleus.h" -o "./nucleus.exe"
```

The binary can then be run in two modes, REPL and File. The REPL can be invoked by calling `./nucleus.exe` with no arguments and ".nuc" files can be executed by calling `./nucleus.exe filename`.

Documentation
-------------

Documentation for Nucleus is currently incomplete.

Quick Examples
--------------

**Hello World:**

```nucleus
std.print("Hello, World!");   # I am a comment :)
```

**Fibonacci:**

```nucleus
reaction fib(n) {
    if (n < 2) return n;
    return fib(n - 2) + fib(n - 1);
}
```

Performance Tests
-----------------

### (Ackermann Function)[https://en.wikipedia.org/wiki/Ackermann_function]
This performance test worked around stress testing function calls with the recursive Ackermann Function. The test was conducted with `ack(3, 6)` as Python cannot compute much higher than this (exceeded max stack allowances). The total iterations completed was 100.

| Language | Average | Min | Max |
|:--------:|:-------:|:---:|:---:|
| **JavaScript** | 1.297ms | 1.092ms | 3.416ms |
| **Python** | 23.473ms | 20.674ms | 27.526ms |
| **Nucleus** | 10.846ms | 10.023ms | 12.968ms |

### Vector Norm (Native Math Calls)
This performance conversly worked on native `math` calls, in particular `math.sqrt` for Nucleus. Additionally, the vector norm method was doubly iterated over to get times for 100000 calls of `vecNorm`.

| Language | Average | Min | Max |
|:--------:|:-------:|:---:|:---:|
| **JavaScript** | 0.119ms | 0.045ms | 2.945ms |
| **Python** | 28.903ms | 26.257ms | 32.302ms |
| **Nucleus** | 11.254ms | 10.007ms | 13.068ms |

Acknowledgements
----------------

I would like to give mention to the (Crafting Interpreters)[https://craftinginterpreters.com/] book. This helped cement a solid foundation for Nucleus, and gave tremendous insight into the creation of C based interpreters.
