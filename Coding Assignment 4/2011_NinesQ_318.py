import math

TARGET = 2011

def solve():
    ans = 0

    for p in range(1, 2011):
        for q in range(p + 1, 2012 - p):

            y = math.sqrt(q) - math.sqrt(p)

            # convergence condition
            if y >= 1:
                continue

            n = int(TARGET / (-2 * math.log10(y))) + 1

            ans += n

    return ans


print(solve())