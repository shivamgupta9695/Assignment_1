from itertools import permutations

def solve():
    nums = set(range(1, 11))
    max_string = ""

    # choose inner ring
    for inner in permutations(range(1, 6)):
        a, b, c, d, e = inner

        # total sum can vary
        for total in range(14, 20):

            o1 = total - a - b
            o2 = total - b - c
            o3 = total - c - d
            o4 = total - d - e
            o5 = total - e - a

            outer = [o1, o2, o3, o4, o5]

            # all outer nodes valid?
            if (
                len(set(outer)) == 5 and
                all(x in nums for x in outer) and
                not any(x in inner for x in outer)
            ):

                # 10 must be outer node for 16-digit answer
                if 10 not in outer:
                    continue

                groups = [
                    (o1, a, b),
                    (o2, b, c),
                    (o3, c, d),
                    (o4, d, e),
                    (o5, e, a)
                ]

                # start from smallest outer node
                min_index = min(range(5), key=lambda i: groups[i][0])

                ordered = groups[min_index:] + groups[:min_index]

                result = ''.join(
                    ''.join(map(str, grp))
                    for grp in ordered
                )

                # only 16-digit strings
                if len(result) == 16:
                    max_string = max(max_string, result)

    return max_string


print(solve())