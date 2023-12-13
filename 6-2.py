example = """
Time:      7  15   30
Distance:  9  40  200
""".strip()
expected = 71503
filename = "6.txt"

from math import prod


def solve(lines):
    t, d = [int(l.replace(" ", "").split(":")[1]) for l in lines]
    return next(t - 2 * tt + 1 for tt in range(t) if tt * (t - tt) > d)


actual = solve(example.split("\n"))
assert actual == expected, f"expected {expected}, got {actual}"

with open(filename) as lines:
    print(solve([l.strip() for l in lines]))
