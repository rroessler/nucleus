import math  # NOTE: Python requires one math import
import time

# Python implementation of Vector Norm
def vecNorm(x, y, z):
    mag = math.sqrt(x * x + y * y + z * z)
    scale = 0
    if mag != 0:
        scale = 1 / mag
    pass


# Python Benchmarker
def bench(iters):
    min = float("inf")
    max = 0
    sum = 0

    for i in range(0, iters):
        start = time.time()
        for j in range(0, 100000):
            vecNorm(2, 8, 2)
        elapsed = time.time() - start  # this is in seconds

        sum = sum + elapsed
        if elapsed < min:
            min = elapsed
        elif elapsed > max:
            max = elapsed

    print("Average: " + str((sum / iters) * 1000) + "ms")
    print("Min: " + str(min * 1000) + "ms")
    print("Max: " + str(max * 1000) + "ms")
    pass


print("\n=> Python3")
bench(75)
print()
