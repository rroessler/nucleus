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

### Install Locally

At this stage the Nucleus language is not available to be installed locally. Although executables to the wrapped NodeJS CLI can be made, install scripts have not yet been implemented. Once this is done however, all that will be needed to run Nucleus source code will be an install script or application
installer.

### NodeJS

The Nucleus project repository can be cloned and ran through NodeJS. This can be done through the following commands.

```bash
git clone https://github.com/rroessler/nucleus.git  # clone the repo
cd nucleus                                          # enter
npm run setup                                       # run the setup items

./nucleus.js "filename.nuc"     # and run a nucleus file
# OR
./nucleus.js                    # enter REPL mdoe
```

### Source

To compile the Nucleus language from source, this repository can be downloaded and the `./src/nucleus.c` file can be compiled as desired. The process to do so can be seen below

```bash
git clone https://github.com/rroessler/nucleus.git
cd nucleus
gcc -O2 -g "./src/nucleus.h" -o "./nucleus.exe" # -O2 flag recommended for performance
```

The binary can then be run in two modes, REPL and File. The REPL can be invoked by calling `./nucleus.exe` with no arguments and ".nuc" files can be executed by calling `./nucleus.exe filename`.

Additionally when compiling from source, some extra "defines" can be utilised to alter Nucleus' functionality. These defines allow for:
- Stack Tracing
- Operation Tracing
- Pre-Execution Bytecode Dump
- Garbage Collection Statistics
- Standard Library Reduction / Selection

In particular the ability to compile a version of Nucleus with reduced standard library functionality will be extremely useful for minimising a Nucleus executable that could be compiled for embedded devices. On top of this, there are plans to seperate the compiler and interpreter so that source bytecode could be compiled and loaded onto an embedded device with a standalone interpreter. Or at least this is what I had in mind when allowing for this implementation.

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

### [Ackermann Function](https://en.wikipedia.org/wiki/Ackermann_function)
This performance test worked around stress testing function calls with the recursive Ackermann Function. The test was conducted with `ack(3, 6)` as Python cannot compute much higher than this (exceeded max stack allowances). The total iterations completed was 100.

<div align="center">

| Language | Average | Min | Max |
|:--------:|:-------:|:---:|:---:|
| **JavaScript** | 1.297ms | 1.092ms | 3.416ms |
| **Python** | 23.473ms | 20.674ms | 27.526ms |
| **Nucleus** | 9.626ms | 9.150ms | 11.734ms |

</div>

### Vector Norm (Native Math Calls)
This performance conversly worked on native `math` calls, in particular `math.sqrt` for Nucleus. Additionally, the vector norm method was doubly iterated over to get times for 100000 calls of `vecNorm`.

<div align="center">

| Language | Average | Min | Max |
|:--------:|:-------:|:---:|:---:|
| **JavaScript** | 0.119ms | 0.045ms | 2.945ms |
| **Python** | 28.903ms | 26.257ms | 32.302ms |
| **Nucleus** | 8.205ms | 7.870ms | 9.684ms |

</div>
    
Acknowledgements
----------------

I would like to give mention to the [Crafting Interpreters](https://craftinginterpreters.com/) book. This helped cement a solid foundation for Nucleus, and gave tremendous insight into the creation of C based interpreters.

License
-------
[MIT](https://opensource.org/licenses/MIT)
