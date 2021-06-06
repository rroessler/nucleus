import time

# Python Implementation of Ackermann Function
def ack(m, n):
    if m == 0:
        return n + 1
    if (m > 0) and (n == 0):
        return ack(m - 1, 1)
    if (m > 0) and (n > 0):
        return ack(m - 1, ack(m, n - 1))
    pass


# Python Benchmarker
def bench(iters):
    min = float("inf")
    max = 0
    sum = 0

    for i in range(0, iters):
        start = time.time()
        ack(3, 6)  # NOTE: Python cannot exceed ack(3, 7)
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
bench(100)
print()
